// SPDX-License-Identifier: MIT

#include "VcuDecodedBufferAdapter.hpp"

extern "C" {
#include "lib_common/BufferMeta.h"
#include "lib_common/FourCC.h"
#include "lib_common/PixMapBuffer.h"
#include "lib_common/Planes.h"
#include "lib_common/PicFormat.h"
#include "lib_common/PixMapBufferInternal.h"
}

namespace ntc_vcu {
namespace {

DecodedNv12View EmptyView()
{
  return DecodedNv12View { nullptr, nullptr, 0U, 0U, 0U, 0U, 0U, 0U };
}

bool IsNv12(AL_TPicFormat const& format)
{
  return format.eChromaMode == AL_CHROMA_4_2_0 &&
         format.eAlphaMode == AL_ALPHA_MODE_DISABLED &&
         format.uBitDepth == 8U &&
         format.eStorageMode == AL_FB_RASTER &&
         format.ePlaneMode == AL_PLANE_MODE_SEMIPLANAR &&
         format.eComponentOrder == AL_COMPONENT_ORDER_YUV &&
         format.eSamplePackMode == AL_SAMPLE_PACK_MODE_BYTE &&
         !format.bCompressed;
}

} // namespace

DecodedBufferAdapterStatus MakeDecodedNv12View(
  AL_TBuffer* pBuffer,
  DecodedNv12View* pView)
{
  if(pView == nullptr)
    return DecodedBufferAdapterStatus::kNullOutput;

  *pView = EmptyView();

  if(pBuffer == nullptr)
    return DecodedBufferAdapterStatus::kNullBuffer;

  if(AL_Buffer_GetMetaData(pBuffer, AL_META_TYPE_PIXMAP) == nullptr)
    return DecodedBufferAdapterStatus::kMissingPixelMetadata;

  TFourCC const fourCC = AL_PixMapBuffer_GetFourCC(pBuffer);
  AL_TPicFormat format {};

  if(!AL_GetPicFormat(fourCC, &format) || !IsNv12(format))
    return DecodedBufferAdapterStatus::kUnsupportedFormat;

  AL_TDimension const dimension = AL_PixMapBuffer_GetDimension(pBuffer);

  if(dimension.iWidth <= 0 || dimension.iHeight <= 0 ||
     (dimension.iWidth & 1) != 0 || (dimension.iHeight & 1) != 0)
    return DecodedBufferAdapterStatus::kInvalidDimensions;

  int32_t const yPitch =
    AL_PixMapBuffer_GetPlanePitch(pBuffer, AL_PLANE_Y);
  int32_t const uvPitch =
    AL_PixMapBuffer_GetPlanePitch(pBuffer, AL_PLANE_UV);

  if(yPitch < dimension.iWidth || uvPitch < dimension.iWidth)
    return DecodedBufferAdapterStatus::kInvalidPitch;

  std::uint8_t* const yPlane =
    AL_PixMapBuffer_GetPlaneAddress(pBuffer, AL_PLANE_Y);
  std::uint8_t* const uvPlane =
    AL_PixMapBuffer_GetPlaneAddress(pBuffer, AL_PLANE_UV);

  if(yPlane == nullptr || uvPlane == nullptr)
    return DecodedBufferAdapterStatus::kMissingPlane;

  pView->yPlane = yPlane;
  pView->uvPlane = uvPlane;
  pView->yPhysicalAddress = static_cast<std::uint64_t>(
    AL_PixMapBuffer_GetPlanePhysicalAddress(pBuffer, AL_PLANE_Y));
  pView->uvPhysicalAddress = static_cast<std::uint64_t>(
    AL_PixMapBuffer_GetPlanePhysicalAddress(pBuffer, AL_PLANE_UV));
  pView->width = static_cast<std::uint32_t>(dimension.iWidth);
  pView->height = static_cast<std::uint32_t>(dimension.iHeight);
  pView->yPitch = static_cast<std::uint32_t>(yPitch);
  pView->uvPitch = static_cast<std::uint32_t>(uvPitch);

  return DecodedBufferAdapterStatus::kOk;
}

} // namespace ntc_vcu
