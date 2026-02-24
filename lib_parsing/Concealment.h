// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_rtos/types.h"

#define AL_MAX_FIRST_LCU_POS_CACHE 32

typedef struct AL_TConceal
{
  bool bHasPPS;
  bool bValidFrame;
  int32_t iLastPPSId;
  int32_t iActivePPS;
  int32_t iFirstLCU;
  bool bSkipRemainingNals;
  uint8_t iSliceDecodeIssued;
  int32_t iFirstLCUStore[AL_MAX_FIRST_LCU_POS_CACHE];
}AL_TConceal;

void AL_Conceal_Init(AL_TConceal* pConceal);
