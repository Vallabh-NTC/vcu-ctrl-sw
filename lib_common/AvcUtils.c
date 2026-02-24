// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_common/AvcUtils.h"
#include "lib_common/SliceConsts.h"

/*************************************************************************/
bool AL_AVC_IsIDR(AL_ENut eNUT)
{
  return eNUT == AL_AVC_NUT_VCL_IDR;
}

/*************************************************************************/
bool AL_AVC_IsVcl(AL_ENut eNUT)
{
  return eNUT == AL_AVC_NUT_VCL_IDR || eNUT == AL_AVC_NUT_VCL_NON_IDR;
}

static AL_ESliceType const AVC_SLICE_TYPE[5] =
{
  AL_SLICE_P, AL_SLICE_B, AL_SLICE_I, AL_SLICE_SP, AL_SLICE_SI
};

AL_ESliceType AL_AVC_ToSliceType(int32_t iSliceType)
{
  if(iSliceType > 9)
    return AL_SLICE_MAX_ENUM;

  return AVC_SLICE_TYPE[iSliceType % 5];
}
