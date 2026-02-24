// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

/******************************************************************************
   \addtogroup lib_encode
   !@{
   \file
 *****************************************************************************/
#pragma once

#include "lib_common/common_syntax_elements.h"
#include "lib_rtos/types.h"

static size_t const AL_SL_INTRA = 0;
static size_t const AL_SL_INTER = 1;
typedef uint32_t AL_TLevels4x4[4 * 4];
typedef uint32_t AL_TLevels8x8[8 * 8];
typedef uint32_t AL_TLevelsDC[4];

/*****************************************************************************
   \brief Scaling List Matrices in hardware preprocessed format
*****************************************************************************/
typedef struct AL_THwScalingList
{
  AL_TLevels8x8 t32x32;
  AL_TLevels8x8 t16x16Y;
  AL_TLevels8x8 t16x16Cb;
  AL_TLevels8x8 t16x16Cr;
  AL_TLevels8x8 t8x8Y;
  AL_TLevels8x8 t8x8Cb;
  AL_TLevels8x8 t8x8Cr;
  AL_TLevels4x4 t4x4Y;
  AL_TLevels4x4 t4x4Cb;
  AL_TLevels4x4 t4x4Cr;
  AL_TLevelsDC tDC;
}AL_THwScalingList;
