// SPDX-License-Identifier: MIT

#pragma once

#include "EncodedSource.hpp"

extern "C"
{
#include "lib_common/Allocator.h"
#include "lib_common/Profiles.h"
}

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ntc
{

class PreloadedFileSource final : public IEncodedSource
{
public:
  PreloadedFileSource(
    AL_TAllocator* pAllocator,
    std::string const& sInputPath,
    std::size_t zMaxAccessUnitSize,
    AL_ECodec eCodec,
    bool bSliceCut);

  bool Next(EncodedAccessUnit& tUnit) override;
  void Reset() override;

  std::size_t GetAccessUnitCount() const;
  std::size_t GetPayloadBytes() const;

private:
  struct BufferDeleter
  {
    void operator()(AL_TBuffer* pBuffer) const;
  };

  using BufferPtr = std::unique_ptr<AL_TBuffer, BufferDeleter>;

  struct StoredAccessUnit
  {
    StoredAccessUnit(
      BufferPtr pBuffer,
      std::size_t zPayloadSize,
      std::uint8_t uFlags);

    BufferPtr pBuffer;
    std::size_t zPayloadSize;
    std::uint8_t uFlags;
  };

  std::vector<StoredAccessUnit> tAccessUnits;
  std::size_t zNextAccessUnit = 0;
  std::size_t zPayloadBytes = 0;
};

} // namespace ntc
