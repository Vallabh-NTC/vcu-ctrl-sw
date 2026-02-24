// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_app/UnCompFrameReader.hpp"

UnCompFrameReader::UnCompFrameReader(std::ifstream& iRecFile, AL_TYUVFileInfo& tFileInfo, bool bLoopFrames) :
  FrameReader(iRecFile, bLoopFrames), m_tFileInfo(tFileInfo), m_uRndDim(DEFAULT_RND_DIM)
{
}

bool UnCompFrameReader::ReadFrame(AL_TBuffer* pBuffer)
{
  return ReadOneFrameYuv(m_recFile, pBuffer, m_bLoopFile, m_uRndDim);
}

void UnCompFrameReader::SeekAbsolute(AL_64U uFrameIdx)
{
  std::streampos iPictSize = GetPictureSize(m_tFileInfo);
  m_recFile.seekg(iPictSize * uFrameIdx, std::ios_base::beg);
}

void UnCompFrameReader::SeekRelative(AL_64S iFrameIdxDelta)
{
  std::streampos iPictSize = GetPictureSize(m_tFileInfo);
  m_recFile.seekg(iPictSize * iFrameIdxDelta, std::ios_base::cur);
}
