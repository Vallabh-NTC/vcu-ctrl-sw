// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_rtos/types.h"
#include "lib_common/PicFormat.h"
#include "lib_common/Allocator.h"

static const int32_t NUMCORE_AUTO = 0;
static const int32_t MAX_BIT_DEPTH_MINUS_8 = 4;
static const int32_t MAX_POC_LSB_MINUS_4 = 12;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (int32_t)(sizeof(x) / sizeof((x)[0]))
#endif

/***************************************************************************/
AL_64U BitsToBytes(AL_64U zBits);

/***************************************************************************/
AL_64U BytesToBits(AL_64U zBytes);

/***************************************************************************/
AL_64U UnsignedClip3(AL_64U uVal, AL_64U uMin, AL_64U uMax);

/***************************************************************************/
AL_64U UnsignedMax(AL_64U uVal1, AL_64U uVal2);

/***************************************************************************/
AL_64U UnsignedMin(AL_64U uVal1, AL_64U uVal2);

/***************************************************************************/
AL_64S Clip3(AL_64S iVal, AL_64S iMin, AL_64S iMax);

/***************************************************************************/
AL_64S Max(AL_64S iVal1, AL_64S iVal2);

/***************************************************************************/
AL_64S Min(AL_64S iVal1, AL_64S iVal2);

/***************************************************************************/
AL_64S Abs(AL_64S iVal);

/***************************************************************************/
AL_64S Sign(AL_64S iVal);

/***************************************************************************/
int32_t ceil_log2(int32_t n);

/****************************************************************************/
int32_t floor_log2(int32_t n);

/****************************************************************************/
int32_t GetBlkNumber(AL_TDimension tDim, uint32_t uBlkWidth, uint32_t uBlkHeight);

/****************************************************************************/
int32_t GetSquareBlkNumber(AL_TDimension tDim, uint32_t uBlkSize);

/****************************************************************************/
int16_t MaxInArray(const int16_t tab[], int32_t arraySize);

/****************************************************************************/
int16_t MinInArray(const int16_t tab[], int32_t arraySize);

/****************************************************************************/
bool IsWindowEmpty(AL_TWindow tWindow);

/****************************************************************************/
AL_HANDLE AlignedAlloc(AL_TAllocator* pAllocator, const char* pBufName, uint32_t uSize, uint32_t uAlign, uint32_t* uAllocatedSize, uint32_t* uAlignmentOffset);

/*****************************************************************************
   \brief Reference picture status
 ***************************************************************************/
typedef enum
{
  SHORT_TERM_REF,
  LONG_TERM_REF,
  UNUSED_FOR_REF,
  NON_EXISTING_REF,
  AL_MARKING_REF_MAX_ENUM, /* sentinel */
}AL_EMarkingRef;
