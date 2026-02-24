// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include "lib_app/Sink.hpp"
#include "lib_common/Profiles.h"

IFrameSink* createRateCtrlMetaSink(std::string const& path, AL_ECodec eCodec);
