// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_common/AxiPropertiesInternal.h"

int32_t convertAxiMaxBurstSizeInRegisterValue(int32_t maxBurstSize)
{
  switch(maxBurstSize)
  {
  case 64:
    return 2;
  case 128:
    return 1;
  case 256:
    return 0;
  case 512:
  default:
    return 3;
  }
}
