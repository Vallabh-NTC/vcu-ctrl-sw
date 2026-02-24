// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_common/SliceConsts.h"
#include "lib_rtos/types.h"
#include "lib_common/Nuts.h"

/*****************************************************************************
   \brief This function checks if the current Nal Unit Type corresponds to an IDR
   picture respect to the AVC specification
   \param[in] eNUT Nal Unit Type
   \return true if it's an IDR nal unit type
   false otherwise
 ***************************************************************************/
bool AL_AVC_IsIDR(AL_ENut eNUT);

/*****************************************************************************
   \brief This function checks if the current Nal Unit Type corresponds to a Vcl NAL
   respect to the AVC specification
   \param[in] eNUT Nal Unit Type
   \return true if it's a VCL nal unit type
   false otherwise
 ***************************************************************************/
bool AL_AVC_IsVcl(AL_ENut eNUT);

/*****************************************************************************
   \brief This function return the AL_ESliceType from the bitstream slice_type
   \param[in] iSliceType slice_type in bitstream
   \return AL_ESliceType or AL_SLICE_MAX_ENUM if slice_type is wrong
   respect to the AVC specification
 ***************************************************************************/
AL_ESliceType AL_AVC_ToSliceType(int32_t iSliceType);

/*****************************************************************************
   \brief Size of the NAL Header
   respect to the AVC specification
 ***************************************************************************/
#define AL_AVC_NAL_HDR_SIZE 4
