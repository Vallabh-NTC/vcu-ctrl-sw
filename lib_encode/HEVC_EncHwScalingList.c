// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "HEVC_EncHwScalingList.h"
#include "lib_rtos/lib_rtos.h"
#include "lib_rtos/types.h"

/****************************************************************************/
static int32_t const AL_SLOW_ENC_HEVC_SCL_ORDER_8x8[64] = // scaling list when 4 samples / cycles in transquant
{
  0, 8, 16, 24,
  32, 40, 48, 56,
  1, 9, 17, 25,
  33, 41, 49, 57,
  2, 10, 18, 26,
  34, 42, 50, 58,
  3, 11, 19, 27,
  35, 43, 51, 59,
  4, 12, 20, 28,
  36, 44, 52, 60,
  5, 13, 21, 29,
  37, 45, 53, 61,
  6, 14, 22, 30,
  38, 46, 54, 62,
  7, 15, 23, 31,
  39, 47, 55, 63
};

/****************************************************************************/
static int32_t const AL_ENC_HEVC_SCL_ORDER_4x4[16] =
{
  0, 4, 8, 12,
  1, 5, 9, 13,
  2, 6, 10, 14,
  3, 7, 11, 15
};

/****************************************************************************/
static const int32_t* pSCL_HEVC_8x8_ORDER = AL_SLOW_ENC_HEVC_SCL_ORDER_8x8;

static const int32_t* pSCL_16x16_ORDER = AL_SLOW_ENC_HEVC_SCL_ORDER_8x8;

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

// quantification scale for HEVC
static int32_t const g_quantScales[6] =
{
  419430, 372827, 328965, 294337, 262144, 233016
};  // 2^24 / level_scale[iQPRem]

/******************************************************************************/
static void AL_HEVC_sGenFwdDC(AL_TSCLParam const* pSclLst, int32_t iQpRem, int32_t iDir, AL_TLevelsDC* pFwd)
{
  (*pFwd)[0] = g_quantScales[iQpRem] / pSclLst->scaling_list_dc_coeff[0][(3 * iDir)];
  (*pFwd)[1] = g_quantScales[iQpRem] / pSclLst->scaling_list_dc_coeff[0][(3 * iDir) + 1];
  (*pFwd)[2] = g_quantScales[iQpRem] / pSclLst->scaling_list_dc_coeff[0][(3 * iDir) + 2];
  (*pFwd)[3] = g_quantScales[iQpRem] / pSclLst->scaling_list_dc_coeff[1][(3 * iDir)];
}

/******************************************************************************/
static void AL_HEVC_sGenFwdLvl4x4(uint8_t const* pMtx, int32_t iQpRem, AL_TLevels4x4* pFwd)
{
  for(int32_t i = 0; i < 16; i++)
    (*pFwd)[i] = g_quantScales[iQpRem] / pMtx[i];
}

/******************************************************************************/
static void AL_HEVC_sGenFwdLvl8x8(uint8_t const* pMtx, int32_t iQpRem, AL_TLevels8x8* pFwd)
{
  for(int32_t i = 0; i < 64; i++)
    (*pFwd)[i] = g_quantScales[iQpRem] / pMtx[i];
}

/******************************************************************************/
void AL_HEVC_GenerateHwScalingList(AL_TSCLParam const* pSclLst, AL_THwScalingList(*pHwSclLst)[2][6])
{
  for(int32_t iQpRem = 0; iQpRem < 6; iQpRem++)
  {
    // Intra
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[3][(3 * AL_SL_INTRA)], iQpRem, &(*pHwSclLst)[0][iQpRem].t32x32);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[2][(3 * AL_SL_INTRA)], iQpRem, &(*pHwSclLst)[0][iQpRem].t16x16Y);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[2][(3 * AL_SL_INTRA) + 1], iQpRem, &(*pHwSclLst)[0][iQpRem].t16x16Cb);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[2][(3 * AL_SL_INTRA) + 2], iQpRem, &(*pHwSclLst)[0][iQpRem].t16x16Cr);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTRA)], iQpRem, &(*pHwSclLst)[0][iQpRem].t8x8Y);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTRA) + 1], iQpRem, &(*pHwSclLst)[0][iQpRem].t8x8Cb);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTRA) + 2], iQpRem, &(*pHwSclLst)[0][iQpRem].t8x8Cr);
    AL_HEVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTRA)], iQpRem, &(*pHwSclLst)[0][iQpRem].t4x4Y);
    AL_HEVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTRA) + 1], iQpRem, &(*pHwSclLst)[0][iQpRem].t4x4Cb);
    AL_HEVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTRA) + 2], iQpRem, &(*pHwSclLst)[0][iQpRem].t4x4Cr);

    AL_HEVC_sGenFwdDC(pSclLst, iQpRem, AL_SL_INTRA, &(*pHwSclLst)[0][iQpRem].tDC);

    // Inter
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[3][(3 * AL_SL_INTER)], iQpRem, &(*pHwSclLst)[1][iQpRem].t32x32);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[2][(3 * AL_SL_INTER)], iQpRem, &(*pHwSclLst)[1][iQpRem].t16x16Y);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[2][(3 * AL_SL_INTER) + 1], iQpRem, &(*pHwSclLst)[1][iQpRem].t16x16Cb);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[2][(3 * AL_SL_INTER) + 2], iQpRem, &(*pHwSclLst)[1][iQpRem].t16x16Cr);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTER)], iQpRem, &(*pHwSclLst)[1][iQpRem].t8x8Y);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTER) + 1], iQpRem, &(*pHwSclLst)[1][iQpRem].t8x8Cb);
    AL_HEVC_sGenFwdLvl8x8(pSclLst->ScalingList[1][(3 * AL_SL_INTER) + 2], iQpRem, &(*pHwSclLst)[1][iQpRem].t8x8Cr);
    AL_HEVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTER)], iQpRem, &(*pHwSclLst)[1][iQpRem].t4x4Y);
    AL_HEVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTER) + 1], iQpRem, &(*pHwSclLst)[1][iQpRem].t4x4Cb);
    AL_HEVC_sGenFwdLvl4x4(pSclLst->ScalingList[0][(3 * AL_SL_INTER) + 2], iQpRem, &(*pHwSclLst)[1][iQpRem].t4x4Cr);

    AL_HEVC_sGenFwdDC(pSclLst, iQpRem, AL_SL_INTER, &(*pHwSclLst)[1][iQpRem].tDC);
  }
}

/******************************************************************************/
void AL_HEVC_WriteEncHwScalingList(AL_TSCLParam const* pSclLst, AL_THwScalingList(*pHwSclLst)[2][6], uint8_t* pBuf)
{
  uint8_t const* pSrcInv;
  uint32_t const* pSrcFwd;
  uint32_t* pBuf32 = (uint32_t*)pBuf;

  // Inverse scaling matrix

  // 32x32 Intra
  pSrcInv = pSclLst->ScalingList[3][0];
  AL_sWriteInvCoeff(pSrcInv, AL_SLOW_ENC_HEVC_SCL_ORDER_8x8, 64, &pBuf32);

  // 32x32 Inter
  pSrcInv = pSclLst->ScalingList[3][3];
  AL_sWriteInvCoeff(pSrcInv, AL_SLOW_ENC_HEVC_SCL_ORDER_8x8, 64, &pBuf32);

  // 16x16 luma Intra
  pSrcInv = pSclLst->ScalingList[2][0];
  AL_sWriteInvCoeff(pSrcInv, pSCL_16x16_ORDER, 64, &pBuf32);

  // 16x16 Cb Intra
  pSrcInv = pSclLst->ScalingList[2][1];
  AL_sWriteInvCoeff(pSrcInv, pSCL_16x16_ORDER, 64, &pBuf32);

  // 16x16 Cr Intra
  pSrcInv = pSclLst->ScalingList[2][2];
  AL_sWriteInvCoeff(pSrcInv, pSCL_16x16_ORDER, 64, &pBuf32);

  // 16x16 luma Inter
  pSrcInv = pSclLst->ScalingList[2][3];
  AL_sWriteInvCoeff(pSrcInv, pSCL_16x16_ORDER, 64, &pBuf32);

  // 16x16 Cb Inter
  pSrcInv = pSclLst->ScalingList[2][4];
  AL_sWriteInvCoeff(pSrcInv, pSCL_16x16_ORDER, 64, &pBuf32);

  // 16x16 Cr Inter
  pSrcInv = pSclLst->ScalingList[2][5];
  AL_sWriteInvCoeff(pSrcInv, pSCL_16x16_ORDER, 64, &pBuf32);

  // 8x8 luma Intra
  pSrcInv = pSclLst->ScalingList[1][0];
  AL_sWriteInvCoeff(pSrcInv, pSCL_HEVC_8x8_ORDER, 64, &pBuf32);

  // 8x8 Cb Intra
  pSrcInv = pSclLst->ScalingList[1][1];
  AL_sWriteInvCoeff(pSrcInv, pSCL_HEVC_8x8_ORDER, 64, &pBuf32);

  // 8x8 Cr Intra
  pSrcInv = pSclLst->ScalingList[1][2];
  AL_sWriteInvCoeff(pSrcInv, pSCL_HEVC_8x8_ORDER, 64, &pBuf32);

  // 8x8 luma Inter
  pSrcInv = pSclLst->ScalingList[1][3];
  AL_sWriteInvCoeff(pSrcInv, pSCL_HEVC_8x8_ORDER, 64, &pBuf32);

  // 8x8 Cb Inter
  pSrcInv = pSclLst->ScalingList[1][4];
  AL_sWriteInvCoeff(pSrcInv, pSCL_HEVC_8x8_ORDER, 64, &pBuf32);

  // 8x8 Cr Inter
  pSrcInv = pSclLst->ScalingList[1][5];
  AL_sWriteInvCoeff(pSrcInv, pSCL_HEVC_8x8_ORDER, 64, &pBuf32);

  // 4x4 Luma Intra
  pSrcInv = pSclLst->ScalingList[0][0];
  AL_sWriteInvCoeff(pSrcInv, AL_ENC_HEVC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cb Intra
  pSrcInv = pSclLst->ScalingList[0][1];
  AL_sWriteInvCoeff(pSrcInv, AL_ENC_HEVC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cr Intra
  pSrcInv = pSclLst->ScalingList[0][2];
  AL_sWriteInvCoeff(pSrcInv, AL_ENC_HEVC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Luma Inter
  pSrcInv = pSclLst->ScalingList[0][3];
  AL_sWriteInvCoeff(pSrcInv, AL_ENC_HEVC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cb Inter
  pSrcInv = pSclLst->ScalingList[0][4];
  AL_sWriteInvCoeff(pSrcInv, AL_ENC_HEVC_SCL_ORDER_4x4, 16, &pBuf32);

  // 4x4 Cr Inter
  pSrcInv = pSclLst->ScalingList[0][5];
  AL_sWriteInvCoeff(pSrcInv, AL_ENC_HEVC_SCL_ORDER_4x4, 16, &pBuf32);

  *pBuf32++ = (pSclLst->scaling_list_dc_coeff[0][(3 * AL_SL_INTRA)]) |
              ((pSclLst->scaling_list_dc_coeff[0][(3 * AL_SL_INTRA) + 1]) << 8) |
              ((pSclLst->scaling_list_dc_coeff[0][(3 * AL_SL_INTRA) + 2]) << 16);

  *pBuf32++ = (pSclLst->scaling_list_dc_coeff[0][(3 * AL_SL_INTER)]) |
              ((pSclLst->scaling_list_dc_coeff[0][(3 * AL_SL_INTER) + 1]) << 8) |
              ((pSclLst->scaling_list_dc_coeff[0][(3 * AL_SL_INTER) + 2]) << 16);

  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;

  *pBuf32++ = (pSclLst->scaling_list_dc_coeff[1][3 * AL_SL_INTRA]) |
              ((pSclLst->scaling_list_dc_coeff[1][3 * AL_SL_INTER]) << 8);
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;

  // Forward scaling matrix (coefficients on 32 bits)

  // 32x32
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
    {
      pSrcFwd = (*pHwSclLst)[m][q].t32x32;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, AL_SLOW_ENC_HEVC_SCL_ORDER_8x8);
    }
  }

  // DC 32x32
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
      *pBuf32++ = (*pHwSclLst)[m][q].tDC[3];
  }

  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;

  // 16x16 luma / Cb / Cr
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
    {
      pSrcFwd = (*pHwSclLst)[m][q].t16x16Y;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_16x16_ORDER);

      pSrcFwd = (*pHwSclLst)[m][q].t16x16Cb;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_16x16_ORDER);

      pSrcFwd = (*pHwSclLst)[m][q].t16x16Cr;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_16x16_ORDER);
    }
  }

  // DC 16x16
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
    {
      *pBuf32++ = (*pHwSclLst)[m][q].tDC[0];
      *pBuf32++ = (*pHwSclLst)[m][q].tDC[1];
      *pBuf32++ = (*pHwSclLst)[m][q].tDC[2];
    }
  }

  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;
  *pBuf32++ = 0;

  // 8x8 luma / Cb / Cr
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
    {
      pSrcFwd = (*pHwSclLst)[m][q].t8x8Y;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_HEVC_8x8_ORDER);

      pSrcFwd = (*pHwSclLst)[m][q].t8x8Cb;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_HEVC_8x8_ORDER);

      pSrcFwd = (*pHwSclLst)[m][q].t8x8Cr;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 16, pSCL_HEVC_8x8_ORDER);
    }
  }

  // 4x4 luma
  for(int32_t q = 0; q < 6; ++q) // QP Modulo 6
  {
    for(int32_t m = 0; m < 2; ++m) // Mode : 0 = Intra; 1 = Inter
    {
      pSrcFwd = (*pHwSclLst)[m][q].t4x4Y;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 4, AL_ENC_HEVC_SCL_ORDER_4x4);

      pSrcFwd = (*pHwSclLst)[m][q].t4x4Cb;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 4, AL_ENC_HEVC_SCL_ORDER_4x4);

      pSrcFwd = (*pHwSclLst)[m][q].t4x4Cr;
      AL_sWriteFwdCoeffs(&pBuf32, pSrcFwd, 4, AL_ENC_HEVC_SCL_ORDER_4x4);
    }
  }
}
