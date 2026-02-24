// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_common/AvcLevels.h"
#include "lib_rtos/types.h"

/*************************************************************************/
bool AL_AVC_IsLevel(int32_t iLevel)
{
  return (iLevel == 9)
         || ((iLevel >= 10) && (iLevel <= 13))
         || ((iLevel >= 20) && (iLevel <= 22))
         || ((iLevel >= 30) && (iLevel <= 32))
         || ((iLevel >= 40) && (iLevel <= 42))
         || ((iLevel >= 50) && (iLevel <= 52))
         || ((iLevel >= 60) && (iLevel <= 62));
}
