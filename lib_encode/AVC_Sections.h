// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once
#include "ITU_Section.h"
#include "IP_EncoderCtx.h"

AL_TNuts CreateAvcNuts(void);
AL_TNalHeader GetNalHeaderAvc(uint8_t uNUT, uint8_t uNalRefIdc, uint8_t uLayerId, uint8_t uTemporalId);
void AVC_GenerateSections(AL_TEncCtx* pCtx, AL_TBuffer* pStream, AL_TEncPicStatus const* pPicStatus, int32_t iPicID, bool bMustWritePPS, bool bMustWriteAUD);
