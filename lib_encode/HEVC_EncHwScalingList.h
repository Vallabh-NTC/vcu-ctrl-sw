// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

/******************************************************************************
   \addtogroup lib_encode
   !@{
   \file
 *****************************************************************************/
#pragma once

#include "ITU_EncHwScalingList.h"
#include "lib_common/common_syntax_elements.h"
#include "lib_rtos/types.h"

/*****************************************************************************
   \brief Converts HEVC Scaling List matrices from software user-friendly format to
   Hardware encoder preprocessed format.
   \param[in]  pSclLst pointer to Scaling List in Software format
   \param[out] pHwSclLst pointer to Hardware formatted Scaling list that receives
   the preprocessed matrices
*****************************************************************************/
void AL_HEVC_GenerateHwScalingList(AL_TSCLParam const* pSclLst, AL_THwScalingList(*pHwSclLst)[2][6]);

/*****************************************************************************
   \brief Dump HEVC hardware formatted encoder scaling list into buffer of bytes
   \param[in]  pSclLst Pointer to the inverse scaling list coefficients
   \param[in]  pHwSclLst Pointer to the forward scaling list coefficients
   \param[out] pBuf Pointer to buffer that receives the scaling list
   matrices data
*****************************************************************************/
void AL_HEVC_WriteEncHwScalingList(AL_TSCLParam const* pSclLst, AL_THwScalingList(*pHwSclLst)[2][6], uint8_t* pBuf);
