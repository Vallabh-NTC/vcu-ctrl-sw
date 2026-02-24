// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

/******************************************************************************
   \addtogroup Driver
   !@{
   \file
 *****************************************************************************/
#pragma once

#include "lib_common/I_Communication.h"

/*****************************************************************************
    \brief Get a driver that will access an hardware device
*****************************************************************************/
AL_ICommunication* AL_GetLinuxDriverCommunication(void);

/*!@}*/
