// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include "lib_app/Sink.hpp"

IFrameSink* createStreamCrcDump(std::string const& path);
