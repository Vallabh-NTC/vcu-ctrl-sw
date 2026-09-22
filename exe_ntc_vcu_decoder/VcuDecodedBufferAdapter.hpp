// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

extern "C" {
#include "lib_common/BufferAPI.h"
}

namespace ntc_vcu {

enum class DecodedBufferAdapterStatus : std::uint8_t
{
  kOk = 0,
  kNullBuffer,
  kNullOutput,
  kMissingPixelMetadata,
  kUnsupportedFormat,
  kInvalidDimensions,
  kInvalidPitch,
  kMissingPlane,
};

// Non-owning view of one writable raster NV12 VCU surface. The adapter never
// allocates, copies, references or releases the underlying AL_TBuffer.
struct DecodedNv12View
{
  std::uint8_t* yPlane;
  std::uint8_t* uvPlane;
  std::uint64_t yPhysicalAddress;
  std::uint64_t uvPhysicalAddress;
  std::uint32_t width;
  std::uint32_t height;
  std::uint32_t yPitch;
  std::uint32_t uvPitch;
};

// Validates that pBuffer describes writable 8-bit raster NV12 and exposes its
// existing planes without copying pixel data. Physical addresses may be zero
// when the active allocator does not provide them.
DecodedBufferAdapterStatus MakeDecodedNv12View(
  AL_TBuffer* pBuffer,
  DecodedNv12View* pView);

} // namespace ntc_vcu
