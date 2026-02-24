// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_common/Profiles.h"

char const* AL_CodecToString(AL_ECodec eCodec)
{
  switch(eCodec)
  {
  case AL_CODEC_AVC: return "AVC/H.264";
  case AL_CODEC_HEVC: return "HEVC/H.265";
  case AL_CODEC_AV1: return "AV1";
  case AL_CODEC_VP9: return "VP9";
  case AL_CODEC_JPEG: return "JPEG";
  case AL_CODEC_VVC: return "VVC/H.266";
  case AL_CODEC_MPEG2: return "MPEG2";
  case AL_CODEC_AVC_I: return "AVC/H.264 Interlaced";
  case AL_CODEC_LCEVC: return "MPEG5/LCEVC";
  case AL_CODEC_AV2: return "AV2";
  case AL_CODEC_JPEG_XS: return "JPEG-XS";
  case AL_CODEC_INVALID: return "Invalid CODEC";
  default: return "Unknown CODEC";
  }
}
