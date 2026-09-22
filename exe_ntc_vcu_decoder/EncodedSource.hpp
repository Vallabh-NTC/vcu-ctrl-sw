// SPDX-License-Identifier: MIT

#pragma once

extern "C"
{
#include "lib_common/BufferAPI.h"
}

#include <cstddef>
#include <cstdint>

namespace ntc
{

struct EncodedAccessUnit
{
  AL_TBuffer* pBuffer = nullptr;
  std::size_t zPayloadSize = 0;
  std::uint8_t uFlags = 0;
};

/*
 * Producer-independent encoded-input boundary.
 *
 * Next() returns false at end of stream. On success, pBuffer must remain
 * valid while the decoder owns its internal reference.
 *
 * Current producer: PreloadedFileSource.
 * Future producer:  PcieDmaSource.
 */
class IEncodedSource
{
public:
  virtual ~IEncodedSource() = default;

  virtual bool Next(EncodedAccessUnit& tUnit) = 0;
  virtual void Reset() = 0;
};

} // namespace ntc
