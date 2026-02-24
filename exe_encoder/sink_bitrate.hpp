// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <memory>
#include "lib_app/Sink.hpp"
#include "CfgParser.hpp"

IFrameSink* createBitrateWriter(std::string path, ConfigFile const& cfg);
