// SPDX-License-Identifier: MIT

#include "PreloadedFileSource.hpp"
#include "InputLoader.hpp"

extern "C"
{
#include "lib_common/BufferMeta.h"
#include "lib_common/BufferSeiMeta.h"
#include "lib_rtos/lib_rtos.h"
}

#include <fstream>
#include <limits>
#include <stdexcept>
#include <utility>

namespace
{

void CloneSeiMetaData(AL_TBuffer* pSource, AL_TBuffer* pDestination)
{
  AL_TMetaData* pSourceMeta =
    AL_Buffer_GetMetaData(pSource, AL_META_TYPE_SEI);

  if(pSourceMeta == nullptr)
    return;

  AL_TMetaData* pClonedMeta = AL_MetaData_Clone(pSourceMeta);

  if(pClonedMeta == nullptr)
    throw std::runtime_error("Failed to clone input SEI metadata");

  if(!AL_Buffer_AddMetaData(pDestination, pClonedMeta))
  {
    AL_MetaData_Destroy(pClonedMeta);
    throw std::runtime_error(
      "Failed to attach input SEI metadata");
  }
}

} // namespace

namespace ntc
{

void PreloadedFileSource::BufferDeleter::operator()(
  AL_TBuffer* pBuffer) const
{
  if(pBuffer != nullptr)
    AL_Buffer_Destroy(pBuffer);
}

PreloadedFileSource::StoredAccessUnit::StoredAccessUnit(
  BufferPtr pOwnedBuffer,
  std::size_t zOwnedPayloadSize,
  std::uint8_t uOwnedFlags)
  : pBuffer(std::move(pOwnedBuffer)),
    zPayloadSize(zOwnedPayloadSize),
    uFlags(uOwnedFlags)
{
}

PreloadedFileSource::PreloadedFileSource(
  AL_TAllocator* pAllocator,
  std::string const& sInputPath,
  std::size_t zMaxAccessUnitSize,
  AL_ECodec eCodec,
  bool bSliceCut)
{
  if(pAllocator == nullptr)
    throw std::invalid_argument("VCU allocator is null");

  if(zMaxAccessUnitSize == 0)
    throw std::invalid_argument(
      "Maximum access-unit size must be non-zero");

  if(zMaxAccessUnitSize >
     static_cast<std::size_t>(
       std::numeric_limits<int32_t>::max()))
  {
    throw std::invalid_argument(
      "Maximum access-unit size exceeds SplitInput limit");
  }

  std::ifstream tInputFile(sInputPath, std::ios::binary);

  if(!tInputFile)
    throw std::runtime_error(
      "Unable to open encoded input file: " + sInputPath);

  BufferPtr pScratchBuffer(
    AL_Buffer_Create_And_AllocateNamed(
      AL_GetDefaultAllocator(),
      zMaxAccessUnitSize,
      nullptr,
      "ntc_preload_scratch"));

  if(!pScratchBuffer)
    throw std::runtime_error(
      "Unable to allocate preloader scratch buffer");

  SplitInput tSplitter(
    static_cast<int32_t>(zMaxAccessUnitSize),
    eCodec,
    bSliceCut);

  while(true)
  {
    std::uint8_t uFlags = 0;

    std::uint32_t uPayloadSize =
      tSplitter.ReadStream(
        tInputFile,
        pScratchBuffer.get(),
        uFlags);

    if(uPayloadSize == 0)
      break;

    BufferPtr pAccessUnit(
      AL_Buffer_Create_And_AllocateNamed(
        pAllocator,
        uPayloadSize,
        nullptr,
        "ntc_preloaded_access_unit"));

    if(!pAccessUnit)
      throw std::runtime_error(
        "Unable to allocate VCU access-unit buffer");

    Rtos_Memcpy(
      AL_Buffer_GetData(pAccessUnit.get()),
      AL_Buffer_GetData(pScratchBuffer.get()),
      uPayloadSize);

    CloneSeiMetaData(
      pScratchBuffer.get(),
      pAccessUnit.get());

    zPayloadBytes += uPayloadSize;

    tAccessUnits.emplace_back(
      std::move(pAccessUnit),
      uPayloadSize,
      uFlags);
  }

  if(tInputFile.bad())
    throw std::runtime_error(
      "I/O error while preloading encoded input");

  if(tAccessUnits.empty())
    throw std::runtime_error(
      "Encoded input contains no access units");
}

bool PreloadedFileSource::Next(EncodedAccessUnit& tUnit)
{
  if(zNextAccessUnit >= tAccessUnits.size())
  {
    tUnit = EncodedAccessUnit {};
    return false;
  }

  StoredAccessUnit& tStored =
    tAccessUnits[zNextAccessUnit++];

  tUnit.pBuffer = tStored.pBuffer.get();
  tUnit.zPayloadSize = tStored.zPayloadSize;
  tUnit.uFlags = tStored.uFlags;

  return true;
}

void PreloadedFileSource::Reset()
{
  zNextAccessUnit = 0;
}

std::size_t PreloadedFileSource::GetAccessUnitCount() const
{
  return tAccessUnits.size();
}

std::size_t PreloadedFileSource::GetPayloadBytes() const
{
  return zPayloadBytes;
}

} // namespace ntc
