// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

/******************************************************************************
   \addtogroup lib_common
   !@{
   \file
 *****************************************************************************/

#include "Utils.h"
#include "lib_common/Round.h"
#include "lib_rtos/assert.h"
#include "lib_rtos/types.h"

/***************************************************************************/
AL_64U BitsToBytes(AL_64U uBits)
{
  return AL_UnsignedRoundUpAndDivide(uBits, 8, 8);
}

/***************************************************************************/
AL_64U BytesToBits(AL_64U uBytes)
{
  return uBytes * 8;
}

/***************************************************************************/
AL_64U UnsignedClip3(AL_64U uVal, AL_64U uMin, AL_64U uMax)
{
  return ((uVal) < (uMin)) ? (uMin) : ((uVal) > (uMax)) ? (uMax) : (uVal);
}

/***************************************************************************/
AL_64S Clip3(AL_64S iVal, AL_64S iMin, AL_64S iMax)
{
  return ((iVal) < (iMin)) ? (iMin) : ((iVal) > (iMax)) ? (iMax) : (iVal);
}

/***************************************************************************/
AL_64S Max(AL_64S iVal1, AL_64S iVal2)
{
  return (iVal1 < iVal2) ? iVal2 : iVal1;
}

/***************************************************************************/
AL_64S Min(AL_64S iVal1, AL_64S iVal2)
{
  return (iVal1 > iVal2) ? iVal2 : iVal1;
}

/***************************************************************************/
AL_64S Abs(AL_64S iVal)
{
  return (iVal > 0) ? iVal : -iVal;
}

/***************************************************************************/
AL_64S Sign(AL_64S iVal)
{
  return (iVal > 0) ? 1 : ((iVal < 0) ? -1 : 0);
}

/***************************************************************************/
AL_64U UnsignedMax(AL_64U uVal1, AL_64U uVal2)
{
  return (uVal1 < uVal2) ? uVal2 : uVal1;
}

/***************************************************************************/
AL_64U UnsignedMin(AL_64U uVal1, AL_64U uVal2)
{
  return (uVal1 > uVal2) ? uVal2 : uVal1;
}

/***************************************************************************/
static int32_t const tab_ceil_log2[] =
{
/*  0.. 7 */
  0, 0, 1, 2, 2, 3, 3, 3,
/*  8..15 */
  3, 4, 4, 4, 4, 4, 4, 4,
/* 16..23 */
  4, 5, 5, 5, 5, 5, 5, 5,
/* 24..31 */
  5, 5, 5, 5, 5, 5, 5, 5
};

/***************************************************************************/
int32_t ceil_log2(int32_t n)
{
  Rtos_Assert(n >= 0);

  if(n < 32)
    return tab_ceil_log2[n];

  int32_t v = 0;

  n--;

  // count the number of bit used to store the decremented n
  while(n != 0)
  {
    n >>= 1;
    ++v;
  }

  return v;
}

/***************************************************************************/
int32_t floor_log2(int32_t n)
{
  int32_t s = -1;

  while(n != 0)
  {
    n >>= 1;
    ++s;
  }

  return s;
}

/****************************************************************************/
int32_t GetBlkNumber(AL_TDimension tDim, uint32_t uBlkWidth, uint32_t uBlkHeight)
{
  return AL_RoundUpAndDivide(tDim.iWidth, uBlkWidth, uBlkWidth) * AL_RoundUpAndDivide(tDim.iHeight, uBlkHeight, uBlkHeight);
}

/****************************************************************************/
int32_t GetSquareBlkNumber(AL_TDimension tDim, uint32_t uBlkSize)
{
  return GetBlkNumber(tDim, uBlkSize, uBlkSize);
}

/****************************************************************************/
AL_HANDLE AlignedAlloc(AL_TAllocator* pAllocator, const char* pBufName, uint32_t uSize, uint32_t uAlign, uint32_t* uAllocatedSize, uint32_t* uAlignmentOffset)
{
  AL_HANDLE pBuf = NULL;
  *uAllocatedSize = 0;
  *uAlignmentOffset = 0;

  uSize += uAlign;

  pBuf = AL_Allocator_AllocNamed(pAllocator, uSize, pBufName);

  if(NULL == pBuf)
    return NULL;

  *uAllocatedSize = uSize;
  AL_PADDR pAddr = AL_Allocator_GetPhysicalAddr(pAllocator, pBuf);
  *uAlignmentOffset = AL_PhysAddrRoundUp(pAddr, uAlign) - pAddr;

  return pBuf;
}

/****************************************************************************/
int16_t MaxInArray(const int16_t tab[], int32_t arraySize)
{
  int16_t max = 0;

  for(int32_t i = 0; i < arraySize; i++)
  {
    max = Max(tab[i], max);
  }

  return max;
}

/****************************************************************************/
int16_t MinInArray(const int16_t tab[], int32_t arraySize)
{
  int16_t min = 0;

  for(int32_t i = 0; i < arraySize; i++)
  {
    min = Min(tab[i], min);
  }

  return min;
}

/****************************************************************************/
bool IsWindowEmpty(AL_TWindow tWindow)
{
  return (tWindow.tDim.iHeight == 0) || (tWindow.tDim.iWidth == 0);
}
