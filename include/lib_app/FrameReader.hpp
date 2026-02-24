// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>
#include <stdexcept>
#include <fstream>

extern "C"
{
#include "lib_common/PicFormat.h"
#include "lib_common/BufferAPI.h"
#include "lib_rtos/types.h"
}

class FrameReader
{
protected:
  std::ifstream& m_recFile;
  bool m_bLoopFile;
  AL_64U m_uTotalFrameCount;

  FrameReader(std::ifstream& iRecFile, bool bLoopFrames) :
    m_recFile(iRecFile),
    m_bLoopFile(bLoopFrames),
    m_uTotalFrameCount(0) {};

public:
  inline AL_64U GetTotalFrameCnt() const { return m_uTotalFrameCount; }

  virtual bool ReadFrame(AL_TBuffer* pFrameBuffer) = 0;

  virtual void SeekAbsolute(AL_64U uFrameIdx) = 0;
  virtual void SeekRelative(AL_64S iFrameIdxDelta) = 0;

  AL_64S GotoNextPicture(int32_t iFileFrameRate, int32_t iEncFrameRate, AL_64S iFilePictCount, AL_64S iEncPictCount);

  size_t GetFileSize();

  virtual ~FrameReader() = default;
};
