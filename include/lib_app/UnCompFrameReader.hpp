// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_app/FrameReader.hpp"
#include "lib_app/YuvIO.hpp"

class UnCompFrameReader : public FrameReader
{
public:
  UnCompFrameReader(std::ifstream& File, AL_TYUVFileInfo& tFileInfo, bool bLoopFrames);
  virtual bool ReadFrame(AL_TBuffer* pFrameBuffer) override;

  void SeekAbsolute(AL_64U uFrameIdx) override;
  void SeekRelative(AL_64S iFrameIdxDelta) override;

  void SetRndDim(uint32_t uRndDim) { m_uRndDim = uRndDim; };

private:
  AL_TYUVFileInfo& m_tFileInfo;
  uint32_t m_uRndDim;
};
