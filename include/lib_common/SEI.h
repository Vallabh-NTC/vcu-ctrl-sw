// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

/******************************************************************************
   \addtogroup high_level_syntax High Level Syntax
   !@{
   \file
 *****************************************************************************/
#pragma once

#include <lib_rtos/types.h>

/****************************************************************************/
typedef enum AL_ESeiFlag
{
  AL_SEI_NONE = 0x00000000, /*!< no SEI */
  // prefix (16 LSBs)
  AL_SEI_BP = 0x00000001, /*!< Buffering period */
  AL_SEI_PT = 0x00000002, /*!< Picture Timing */
  AL_SEI_RP = 0x00000004, /*!< Recovery Point */
  AL_SEI_MDCV = 0x0000008, /*!< Mastering Display Colour Volume (HDR) */
  AL_SEI_CLL = 0x00000010, /*!< Content Light Level (HDR) */
  AL_SEI_ATC = 0x00000020, /*!< Alternative Transfer Characteristics (HDR) */
  AL_SEI_ST2094_10 = 0x00000040, /*!< ST2094_10 (Dynamic HDR) */
  AL_SEI_ST2094_40 = 0x00000080, /*!< ST2094_40 (Dynamic HDR) */
  // suffix (16 MSBs)

  AL_SEI_ALL = ~0x0, /*!< All supported SEI */
}AL_ESeiFlag;

/****************************************************************************/
static inline bool AL_HAS_SEI_SUFFIX(AL_ESeiFlag seiFlag)
{
  return (seiFlag & 0xFFFF0000) != 0;
}

/****************************************************************************/
static inline bool AL_HAS_SEI_PREFIX(AL_ESeiFlag seiFlag)
{
  return (seiFlag & 0x0000FFFF) != 0;
}

/*!@}*/
