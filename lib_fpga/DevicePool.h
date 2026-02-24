// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_rtos/types.h"

int32_t AL_DevicePool_Open(char const* filename);
int32_t AL_DevicePool_Close(int32_t fd);
