// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "VcuDecodedBufferAdapter.hpp"
#include "face_plate_nv12_roi_worker.h"

namespace ntc_vcu {

// Decoder-context-owned integration for a decoded VCU NV12 buffer. The first
// target qualification executes the four jobs synchronously; the gate and
// buffer ownership contract stay the same when Bosch supplies core dispatch.
class FacePlateRuntime {
public:
  bool Initialize(const std::string& weightsDirectory);
  bool Process(AL_TBuffer* frame, std::uint32_t cropLeft,
               std::uint32_t cropTop, std::uint32_t cropRight,
               std::uint32_t cropBottom);

private:
  plate_native::PlateBackboneWeights weights_;
  std::array<hyper_minimal::FacePlateNv12RoiWorker, 3> workers_;
  std::uint32_t roiWidth_ = 0;
  std::uint32_t roiHeight_ = 0;
  std::uint64_t frameId_ = 0;
  bool ready_ = false;
};

} // namespace ntc_vcu
