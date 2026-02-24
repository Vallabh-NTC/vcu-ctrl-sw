// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_app/FrameReader.hpp"
#include "lib_app/FileUtils.hpp"

extern "C"
{
#include "lib_rtos/types.h"
}

size_t FrameReader::GetFileSize(void)
{
  size_t zSize;

  if(!::GetFileSize(m_recFile, zSize))
    throw std::runtime_error("Invalid YUV file");
  return zSize;
}

AL_64S FrameReader::GotoNextPicture(int32_t iFileFrameRate, int32_t iEncFrameRate, AL_64S iEncPictCount, AL_64S iFilePictCount)
{
  const AL_64S iMove = ((iEncPictCount * iFileFrameRate) / iEncFrameRate) - iFilePictCount;

  if(iMove)
    this->SeekRelative(iMove);

  return iMove;
}
