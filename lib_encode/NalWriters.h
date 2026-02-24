// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_bitstream/IRbspWriter.h"
#include "IP_Stream.h"
#include "lib_common/SEI.h"

typedef struct AL_TNalUnit
{
  void (* Write)(IRbspWriter* writer, AL_TBitStreamLite* bitstream, void const* param, int32_t layerId);
  void const* param;
  int32_t nut;
  int32_t nalRefIdc;
  int32_t layerId;
  int32_t tempId;
  AL_TNalHeader header;
}AL_TNalUnit;

AL_TNalUnit AL_CreateAud(int32_t nut, AL_TAud* aud, int32_t tempId);
AL_TNalUnit AL_CreateSps(int32_t nut, AL_TSps* sps, int32_t layerId, int32_t tempId);
AL_TNalUnit AL_CreatePps(int32_t nut, AL_TPps* pps, int32_t layerId, int32_t tempId);
AL_TNalUnit AL_CreateVps(int32_t nut, AL_TVps* vps, int32_t tempId);

#include "lib_common_enc/EncPicInfo.h"
typedef struct AL_TSeiPrefixAPSCtx
{
  AL_TSps* sps;
  AL_THevcVps* vps;
}AL_TSeiPrefixAPSCtx;

AL_TNalUnit AL_CreateSeiPrefixAPS(AL_TSeiPrefixAPSCtx* ctx, int32_t nut, int32_t layerId, int32_t tempId);

typedef struct AL_TSeiPrefixCtx
{
  AL_TSps* sps;
  int32_t cpbInitialRemovalDelay;
  int32_t cpbRemovalDelay;
  AL_ESeiFlag eFlags;
  AL_TEncPicStatus const* pPicStatus;
  AL_THDRSEIs* pHDRSEIs;
}AL_TSeiPrefixCtx;

AL_TNalUnit AL_CreateSeiPrefix(AL_TSeiPrefixCtx* ctx, int32_t nut, int32_t layerId, int32_t tempId);

typedef struct AL_TAllegroNumSlicesSeiCtx
{
  int8_t numSlices;
}AL_TAllegroNumSlicesSeiCtx;

AL_TNalUnit AL_CreateAllegroNumSlicesSei(AL_TAllegroNumSlicesSeiCtx* ctx, int32_t nut, int32_t layerId, int32_t tempId);

typedef struct AL_TSeiExternalCtx
{
  uint8_t* pPayload;
  int32_t iPayloadType;
  int32_t iPayloadSize;
}AL_TSeiExternalCtx;

AL_TNalUnit AL_CreateExternalSei(AL_TSeiExternalCtx* ctx, int32_t nut, int32_t layerId, int32_t tempId);
