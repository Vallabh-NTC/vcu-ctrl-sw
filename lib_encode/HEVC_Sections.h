// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once
#include "ITU_Section.h"
#include "IP_EncoderCtx.h"

AL_TNuts CreateHevcNuts(void);
void HEVC_GenerateSections(AL_TEncCtx* pCtx, AL_TBuffer* pStream, AL_TEncPicStatus const* pPicStatus, int32_t iLayerID, int32_t iPicID, bool bMustWritePPS, bool bMustWriteAUD);
