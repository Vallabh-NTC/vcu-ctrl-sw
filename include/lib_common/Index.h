// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_rtos/types.h"

#define AL_TIndex int32_t
#define AL_BAD_INDEX ((AL_TIndex)(-1))

static inline bool AL_IS_VALID_INDEX(AL_TIndex tIndex)
{
  return tIndex > AL_BAD_INDEX;
}
