// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "AVC_EncHwScalingList.h"
#include "lib_rtos/lib_rtos.h"
#include "lib_rtos/types.h"

/****************************************************************************/
static int32_t const AL_SLOW_AVC_ENC_SCL_ORDER_8x8[64] = // scaling list when 4 samples / cycles in transquant
{
  0, 1, 2, 3,
  4, 5, 6, 7,
  8, 9, 10, 11,
  12, 13, 14, 15,
  16, 17, 18, 19,
  20, 21, 22, 23,
  24, 25, 26, 27,
  28, 29, 30, 31,
  32, 33, 34, 35,
  36, 37, 38, 39,
  40, 41, 42, 43,
  44, 45, 46, 47,
  48, 49, 50, 51,
  52, 53, 54, 55,
  56, 57, 58, 59,
  60, 61, 62, 63
};

/****************************************************************************/
static int32_t const AL_AVC_ENC_SCL_ORDER_4x4[16] =
{
  0, 1, 2, 3,
  4, 5, 6, 7,
  8, 9, 10, 11,
  12, 13, 14, 15
};

/****************************************************************************/
static const int32_t* pSCL_AVC_8x8_ORDER = AL_SLOW_AVC_ENC_SCL_ORDER_8x8;

/******************************************************************************/
static void AL_sWriteFwdCoeffs(uint32_t** ppBuf, const uint32_t* pSrc, int32_t iSize, const int32_t* pScan)
{
  uint32_t* pCoeff = *ppBuf;

  for(int32_t scl = 0; scl < iSize; ++scl)
  {
    int32_t iOffset = scl << 2;

    *pCoeff++ = pSrc[pScan ? pScan[iOffset] : iOffset];
    *pCoeff++ = pSrc[pScan ? pScan[iOffset + 1] : iOffset + 1];
    *pCoeff++ = pSrc[pScan ? pScan[iOffset + 2] : iOffset + 2];
    *pCoeff++ = pSrc[pScan ? pScan[iOffset + 3] : iOffset + 3];
  }

  *ppBuf = pCoeff;
}

/******************************************************************************/
static void AL_sWriteInvCoeff(const uint8_t* pSrc, const int32_t* pScan, int32_t iSize, uint32_t** pBuf)
{
  int32_t iNumWord = iSize / sizeof(uint32_t);

  for(int32_t scl = 0; scl < iNumWord; ++scl)
  {
    int32_t uOffset = scl << 2;
    (*pBuf)[scl] = pSrc[pScan ? pScan[uOffset] : uOffset] |
                   (pSrc[pScan ? pScan[uOffset + 1] : uOffset + 1] << 8) |
                   (pSrc[pScan ? pScan[uOffset + 2] : uOffset + 2] << 16) |
                   (pSrc[pScan ? pScan[uOffset + 3] : uOffset + 3] << 24);
  }

  *pBuf += iNumWord;
}

#define  X16_DIV(n, d) ((((n) << 4) + ((d) >> 1)) / (d))

static void AL_AVC_sGenFwdLvl4x4(uint8_t const* pMtx, int32_t iQpRem, AL_TLevels4x4* pFwd)
{
  static const uint16_t quant_coef4[6][16] =
  {
    { 13107, 8066, 13107, 8066, 8066, 5243, 8066, 5243, 13107, 8066, 13107, 8066, 8066, 5243, 8066, 5243 },
    { 11916, 7490, 11916, 7490, 7490, 4660, 7490, 4660, 11916, 7490, 11916, 7490, 7490, 4660, 7490, 4660 },
    { 10082, 6554, 10082, 6554, 6554, 4194, 6554, 4194, 10082, 6554, 10082, 6554, 6554, 4194, 6554, 4194 },
    { 9362, 5825, 9362, 5825, 5825, 3647, 5825, 3647, 9362, 5825, 9362, 5825, 5825, 3647, 5825, 3647 },
    { 8192, 5243, 8192, 5243, 5243, 3355, 5243, 3355, 8192, 5243, 8192, 5243, 5243, 3355, 5243, 3355 },
    { 7282, 4559, 7282, 4559, 4559, 2893, 4559, 2893, 7282, 4559, 7282, 4559, 4559, 2893, 4559, 2893 }
  };

  for(int32_t i = 0; i < 16; i++)
  {
    (*pFwd)[i] = X16_DIV(quant_coef4[iQpRem][i], pMtx[i]);
  }
}

/******************************************************************************/
static void AL_AVC_sGenFwdLvl8x8(uint8_t const* pMtx, int32_t iQpRem, AL_TLevels8x8* pFwd)
{
  static const uint16_t quant_coef8[6][64] =
  {
    {
      13107, 12222, 16777, 12222, 13107, 12222, 16777, 12222,
      12222, 11428, 15481, 11428, 12222, 11428, 15481, 11428,
      16777, 15481, 20972, 15481, 16777, 15481, 20972, 15481,
      12222, 11428, 15481, 11428, 12222, 11428, 15481, 11428,
      13107, 12222, 16777, 12222, 13107, 12222, 16777, 12222,
      12222, 11428, 15481, 11428, 12222, 11428, 15481, 11428,
      16777, 15481, 20972, 15481, 16777, 15481, 20972, 15481,
      12222, 11428, 15481, 11428, 12222, 11428, 15481, 11428
    },
    {
      11916, 11058, 14980, 11058, 11916, 11058, 14980, 11058,
      11058, 10826, 14290, 10826, 11058, 10826, 14290, 10826,
      14980, 14290, 19174, 14290, 14980, 14290, 19174, 14290,
      11058, 10826, 14290, 10826, 11058, 10826, 14290, 10826,
      11916, 11058, 14980, 11058, 11916, 11058, 14980, 11058,
      11058, 10826, 14290, 10826, 11058, 10826, 14290, 10826,
      14980, 14290, 19174, 14290, 14980, 14290, 19174, 14290,
      11058, 10826, 14290, 10826, 11058, 10826, 14290, 10826
    },
    {
      10082, 9675, 12710, 9675, 10082, 9675, 12710, 9675,
      9675, 8943, 11985, 8943, 9675, 8943, 11985, 8943,
      12710, 11985, 15978, 11985, 12710, 11985, 15978, 11985,
      9675, 8943, 11985, 8943, 9675, 8943, 11985, 8943,
      10082, 9675, 12710, 9675, 10082, 9675, 12710, 9675,
      9675, 8943, 11985, 8943, 9675, 8943, 11985, 8943,
      12710, 11985, 15978, 11985, 12710, 11985, 15978, 11985,
      9675, 8943, 11985, 8943, 9675, 8943, 11985, 8943
    },
    {
      9362, 8931, 11984, 8931, 9362, 8931, 11984, 8931,
      8931, 8228, 11259, 8228, 8931, 8228, 11259, 8228,
      11984, 11259, 14913, 11259, 11984, 11259, 14913, 11259,
      8931, 8228, 11259, 8228, 8931, 8228, 11259, 8228,
      9362, 8931, 11984, 8931, 9362, 8931, 11984, 8931,
      8931, 8228, 11259, 8228, 8931, 8228, 11259, 8228,
      11984, 11259, 14913, 11259, 11984, 11259, 14913, 11259,
      8931, 8228, 11259, 8228, 8931, 8228, 11259, 8228
    },
    {
      8192, 7740, 10486, 7740, 8192, 7740, 10486, 7740,
      7740, 7346, 9777, 7346, 7740, 7346, 9777, 7346,
      10486, 9777, 13159, 9777, 10486, 9777, 13159, 9777,
      7740, 7346, 9777, 7346, 7740, 7346, 9777, 7346,
      8192, 7740, 10486, 7740, 8192, 7740, 10486, 7740,
      7740, 7346, 9777, 7346, 7740, 7346, 9777, 7346,
      10486, 9777, 13159, 9777, 10486, 9777, 13159, 9777,
      7740, 7346, 9777, 7346, 7740, 7346, 9777, 7346
    },
    {
      7282, 6830, 9118, 6830, 7282, 6830, 9118, 6830,
      6830, 6428, 8640, 6428, 6830, 6428, 8640, 6428,
      9118, 8640, 11570, 8640, 9118, 8640, 11570, 8640,
      6830, 6428, 8640, 6428, 6830, 6428, 8640, 6428,
      7282, 6830, 9118, 6830, 7282, 6830, 9118, 6830,
      6830, 6428, 8640, 6428, 6830, 6428, 8640, 6428,
      9118, 8640, 11570, 8640, 9118, 8640, 11570, 8640,
      6830, 6428, 8640, 6428, 6830, 6428, 8640, 6428
    }
  };

  for(int32_t i = 0; i < 64; i++)
  {
    (*pFwd)[i] = X16_DIV(quant_coef8[iQpRem][i], pMtx[i]);
  }
}

/******************************************************************************/
void AL_AVC_GenerateHwScalingList(AL_TSCLParam const* pSclLst, uint8_t chroma_format_idc, AL_THwScalingList(*pHwSclLst)[2][6])
{
  for(int32_t iQpRem = 0; iQpRem < 6; iQpRem++)
  {
    // 4x4
    AL_AVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTRA)], iQpRem, &(*pHwSclLst)[0][iQpRem].t4x4Y);
    AL_AVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTRA) + 1], iQpRem, &(*pHwSclLst)[0][iQpRem].t4x4Cb);
    AL_AVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTRA) + 2], iQpRem, &(*pHwSclLst)[0][iQpRem].t4x4Cr);
    AL_AVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTER)], iQpRem, &(*pHwSclLst)[1][iQpRem].t4x4Y);
    AL_AVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTER) + 1], iQpRem, &(*pHwSclLst)[1][iQpRem].t4x4Cb);
    AL_AVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTER) + 2], iQpRem, &(*pHwSclLst)[1][iQpRem].t4x4Cr);

    // 8x8
    AL_AVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTRA)], iQpRem, &(*pHwSclLst)[0][iQpRem].t8x8Y);
    AL_AVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTER)], iQpRem, &(*pHwSclLst)[1][iQpRem].t8x8Y);

    if(chroma_format_idc == 3)
    {
      AL_AVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTRA) + 1], iQpRem, &(*pHwSclLst)[0][iQpRem].t8x8Cb);
      AL_AVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTRA) + 2], iQpRem, &(*pHwSclLst)[0][iQpRem].t8x8Cr);
      AL_AVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTER) + 1], iQpRem, &(*pHwSclLst)[1][iQpRem].t8x8Cb);
      AL_AVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTER) + 2], iQpRem, &(*pHwSclLst)[1][iQpRem].t8x8Cr);
    }
  }
}

/******************************************************************************/
void AL_AVC_WriteEncHwScalingList(AL_TSCLParam const* pSclLst, AL_THwScalingList(*pHwSclLst)[2][6], uint8_t chroma_format_idc, uint8_t* pBuf)
{
  uint8_t const* pSrcInv;
  uint32_t const* pSrcFwd;
  uint32_t* pBuf32 = (uint32_t*)pBuf;

  Rtos_Assert((1 & (size_t)pBuf) == 0);

  // Inverse scaling matrix

  // 8x8 luma Intra
  pSrcInv = pSclLst->ScalingList[1][0];
  AL_sWriteInvCoeff(pSrcInv, pSCL_AVC_8x8_ORDER, 64, &pBuf32);

  if(chroma_format_idc == 3)
  {
    // 8x8 Cb Intra
    pSrcInv = pSclLst->ScalingList[1][1];
    AL_sWriteInvCoeff(pSrcInv, pSCL_AVC_8x8_ORDER, 64, &pBuf32);

    // 8x8 Cr Intra
    pSrcInv = pSclLst->ScalingList[1][2];
    AL_sWriteInvCoeff(pSrcInv, pSCL_AVC_8x8_ORDER, 64, &pBuf32);
  }

  // 8x8 luma Inter
  pSrcInv = pSclLst->ScalingList[1][3];
  AL_sWriteInvCoeff(pSrcInv, pSCL_AVC_8x8_ORDER, 64, &pBuf32);

  if(chroma_format_idc == 3)
  {
    // 8x8 Cb Inter
    pSrcInv = pSclLst->ScalingList[1][4];
    AL_sWriteInvCoeff(pSrcInv, pSCL_AVC_8x8_ORDER, 64, &pBuf32);

    // 8x8 Cr Inter
    pSrcInv = pSclLst->ScalingList[1][5];
    AL_sWriteInvCoeff(pSrcInv, pSCL_AVC_8x8_ORDER, 64, &pBuf32);
  }

  // 4x4 Luma Intra
  pSrcInv = pSclLst->ScalingList[0][0];
  AL_sWriteInvCoeff(pSrcInv, AL_AVC_ENC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cb Intra
  pSrcInv = pSclLst->ScalingList[0][1];
  AL_sWriteInvCoeff(pSrcInv, AL_AVC_ENC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cr Intra
  pSrcInv = pSclLst->ScalingList[0][2];
  AL_sWriteInvCoeff(pSrcInv, AL_AVC_ENC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Luma Inter
  pSrcInv = pSclLst->ScalingList[0][3];
  AL_sWriteInvCoeff(pSrcInv, AL_AVC_ENC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cb Inter
  pSrcInv = pSclLst->ScalingList[0][4];
  AL_sWriteInvCoeff(pSrcInv, AL_AVC_ENC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cr Inter
  pSrcInv = pSclLst->ScalingList[0][5];
  AL_sWriteInvCoeff(pSrcInv, AL_AVC_ENC_SCL_ORDER_4x4, 16, &pBuf32);

  // Forward scaling Matrix
  // 8x8 luma / Cb / Cr
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
    {
      pSrcFwd = (*pHwSclLst)[m][q].t8x8Y;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_AVC_8x8_ORDER);

      if(chroma_format_idc == 3)
      {
        pSrcFwd = (*pHwSclLst)[m][q].t8x8Cb;
        AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_AVC_8x8_ORDER);

        pSrcFwd = (*pHwSclLst)[m][q].t8x8Cr;
        AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_AVC_8x8_ORDER);
      }
    }
  }

  // 4x4 luma
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
    {
      pSrcFwd = (*pHwSclLst)[m][q].t4x4Y;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 4, AL_AVC_ENC_SCL_ORDER_4x4);

      pSrcFwd = (*pHwSclLst)[m][q].t4x4Cb;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 4, AL_AVC_ENC_SCL_ORDER_4x4);

      pSrcFwd = (*pHwSclLst)[m][q].t4x4Cr;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 4, AL_AVC_ENC_SCL_ORDER_4x4);
    }
  }
}
