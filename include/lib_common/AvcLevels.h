// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_rtos/types.h"

/*****************************************************************************
   \brief This function checks if the current level corresponds to a level in
   respect to the AVC specification
   \param[in] iLevel level
   \return true if it's a correct level
   false otherwise
 ***************************************************************************/
bool AL_AVC_IsLevel(int32_t iLevel);
