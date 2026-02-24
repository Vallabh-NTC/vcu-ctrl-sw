// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <cstdarg>
#include <mutex>
#include "lib_app/utils.hpp"

using namespace std;

int32_t g_Verbosity = 10;

static void Message(EConColor Color, char const* sMsg, va_list args)
{
  static std::mutex s_LogMutex;
  std::lock_guard<std::mutex> guard(s_LogMutex);
  SetConsoleColor(Color);
  vfprintf(stdout, sMsg, args);
  fflush(stdout);
  SetConsoleColor(CC_DEFAULT);
}

void LogError(char const* sMsg, ...)
{
  if(g_Verbosity < 1)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(CC_RED, sMsg, args);
  va_end(args);
}

void LogWarning(char const* sMsg, ...)
{
  if(g_Verbosity < 3)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(CC_YELLOW, sMsg, args);
  va_end(args);
}

void LogDimmedWarning(char const* sMsg, ...)
{
  if(g_Verbosity < 4)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(CC_GREY, sMsg, args);
  va_end(args);
}

void LogInfo(char const* sMsg, ...)
{
  if(g_Verbosity < 5)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(CC_DEFAULT, sMsg, args);
  va_end(args);
}

void LogInfo(EConColor Color, char const* sMsg, ...)
{
  if(g_Verbosity < 5)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(Color, sMsg, args);
  va_end(args);
}

void LogVerbose(char const* sMsg, ...)
{
  if(g_Verbosity < 7)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(CC_DEFAULT, sMsg, args);
  va_end(args);
}

void LogVerbose(EConColor Color, char const* sMsg, ...)
{
  if(g_Verbosity < 7)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(Color, sMsg, args);
  va_end(args);
}

void LogDebug(char const* sMsg, ...)
{
  if(g_Verbosity < 20)
    return;

  va_list args;
  va_start(args, sMsg);
  Message(CC_DEFAULT, sMsg, args);
  va_end(args);
}

void OpenInput(std::ifstream& fp, std::string const& filename, bool binary)
{
  fp.open(filename, binary ? std::ios::binary : std::ios::in);
  fp.exceptions(ifstream::badbit);

  if(!fp.is_open())
    throw std::runtime_error("Can't open file for reading: '" + filename + "'");
}

void OpenOutput(std::ofstream& fp, std::string const& filename, bool binary)
{
  auto open_mode = binary ? std::ios::out | std::ios::binary : std::ios::out;

  fp.open(filename, open_mode);
  fp.exceptions(ofstream::badbit);

  if(!fp.is_open())
    throw std::runtime_error("Can't open file for writing: '" + filename + "'");
}

const std::string VersionToStr(uint32_t const& version)
{
  std::stringstream ss;
  ss << std::to_string(static_cast<uint8_t>(version >> 20)) << ".";
  ss << std::to_string(static_cast<uint8_t>(version >> 12)) << ".";
  ss << std::to_string(static_cast<uint8_t>(version));

  return ss.str();
}
