// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_common/HevcLevels.h"
#include "lib_rtos/types.h"

/****************************************************************************/
bool AL_HEVC_IsLevel(int32_t iLevel)
{
  return (iLevel == 10)
         || ((iLevel >= 20) && (iLevel <= 21))
         || ((iLevel >= 30) && (iLevel <= 31))
         || ((iLevel >= 40) && (iLevel <= 41))
         || ((iLevel >= 50) && (iLevel <= 52))
         || ((iLevel >= 60) && (iLevel <= 63))
         || ((iLevel >= 70) && (iLevel <= 72));
}
