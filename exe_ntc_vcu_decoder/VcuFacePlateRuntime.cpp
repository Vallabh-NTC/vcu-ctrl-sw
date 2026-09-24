// SPDX-License-Identifier: MIT
#include "VcuFacePlateRuntime.hpp"

#include <array>
#include <limits>

namespace ntc_vcu {

bool FacePlateRuntime::Initialize(const std::string& weightsDirectory)
{
  ready_ = false;
  if(weightsDirectory.empty())
    return false;
  ready_ = weights_.LoadDirectory(weightsDirectory);
  return ready_;
}

bool FacePlateRuntime::Process(AL_TBuffer* frame, std::uint32_t cropLeft,
                               std::uint32_t cropTop, std::uint32_t cropRight,
                               std::uint32_t cropBottom)
{
  if(!ready_)
    return false;

  DecodedNv12View decoded {};
  if(MakeDecodedNv12View(frame, &decoded) != DecodedBufferAdapterStatus::kOk ||
     cropLeft >= decoded.width || cropRight >= decoded.width - cropLeft ||
     cropTop >= decoded.height || cropBottom >= decoded.height - cropTop ||
     decoded.width > static_cast<std::uint32_t>(std::numeric_limits<int>::max()) ||
     decoded.height > static_cast<std::uint32_t>(std::numeric_limits<int>::max()))
    return false;

  const std::uint32_t visibleWidth = decoded.width - cropLeft - cropRight;
  const std::uint32_t visibleHeight = decoded.height - cropTop - cropBottom;
  // Each ROI must own complete, disjoint 2x2 NV12 chroma cells.
  if((cropLeft & 1U) != 0U || (cropTop & 1U) != 0U ||
     visibleWidth % 4U != 0U || visibleHeight % 4U != 0U)
    return false;
  const std::uint32_t halfWidth = visibleWidth / 2U;
  const std::uint32_t halfHeight = visibleHeight / 2U;
  if(halfWidth != roiWidth_ || halfHeight != roiHeight_)
  {
    ready_ = false;
    for(auto& worker : workers_)
      if(!worker.Initialize(static_cast<int>(halfWidth),
                            static_cast<int>(halfHeight)))
        return false;
    roiWidth_ = halfWidth;
    roiHeight_ = halfHeight;
    ready_ = true;
  }

  const hyper_minimal::Nv12ImageView view {
    decoded.yPlane, decoded.uvPlane,
    static_cast<std::uintptr_t>(decoded.yPhysicalAddress),
    static_cast<std::uintptr_t>(decoded.uvPhysicalAddress),
    decoded.width, decoded.height, decoded.yPitch, decoded.uvPitch,
  };
  const int w = static_cast<int>(halfWidth);
  const int h = static_cast<int>(halfHeight);
  const int x = static_cast<int>(cropLeft);
  const int y = static_cast<int>(cropTop);
  const std::array<hyper_minimal::ImageRect, 4> rois {{{
    x, y, w, h}, {x + w, y, w, h},
    {x, y + h, w, h}, {x + w, y + h, w, h},
  }};
  const std::array<std::size_t, 4> assignment {{0, 1, 2, 0}};
  const auto identity = reinterpret_cast<std::uintptr_t>(frame);
  if(identity == 0 || frameId_ == std::numeric_limits<std::uint64_t>::max())
    return false;
  const auto id = ++frameId_;
  hyper_minimal::FacePlateFrameGate gate(id, identity);

  for(std::size_t roi = 0; roi < rois.size(); ++roi)
  {
    hyper_minimal::FacePlateJobCompletion completion {};
    if(workers_[assignment[roi]].Run(
         view, rois[roi], id, identity, static_cast<std::uint32_t>(roi),
         weights_, 0.20f, 0.50f, &completion) != 0 ||
       !gate.Record(completion))
      return false;
  }
  // Caller flushes the original AL_TBuffer only after this single-use gate.
  return gate.TakeAuthorization();
}

} // namespace ntc_vcu
