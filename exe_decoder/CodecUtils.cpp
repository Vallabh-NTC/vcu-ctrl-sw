// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "CodecUtils.hpp"
#include "lib_app/utils.hpp"
#include "lib_app/BuildInfo.hpp"

extern "C" {
#include "resource.h"
}

/******************************************************************************/
void DisplayFrameStatus(AL_64S iFrameNum)
{
#if VERBOSE_MODE
  LogVerbose("\n\n> % 3ld", iFrameNum);
#else
  LogVerbose("\r  Displayed picture #%-6ld - ", iFrameNum);
#endif
}

#if !HAS_COMPIL_FLAGS
#define AL_COMPIL_FLAGS ""
#endif

void DisplayBuildInfo(void)
{
  BuildInfoDisplay displayBuildInfo {
    SCM_REV_SW, SCM_BRANCH, AL_CONFIGURE_COMMANDLINE, AL_COMPIL_FLAGS, DELIVERY_BUILD_NUMBER, DELIVERY_SCM_REV, DELIVERY_DATE
  };
  displayBuildInfo();
}

/*****************************************************************************/
void DisplayVersionInfo(void)
{
  DisplayVersionInfo(AL_DECODER_COMPANY,
                     AL_DECODER_PRODUCT_NAME,
                     AL_DECODER_VERSION,
                     AL_DECODER_COPYRIGHT,
                     AL_DECODER_COMMENTS);
}
