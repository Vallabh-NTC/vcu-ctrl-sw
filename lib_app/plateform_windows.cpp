// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_app/plateform.hpp"

#include <windows.h>

void InitializePlateform(void)
{
  SetErrorMode(SetErrorMode(0) | SEM_NOGPFAULTERRORBOX);
}
