// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// Windows
#include "framework.h"
#include "ddraw.h"
#include "mmeapi.h"
#include "./dsound.h"

// STL
#include <iostream>
#include <algorithm>
#include <string>
#include <string_view>
#include <cstddef>

// External
#include <keystone/keystone.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>
#include <MinHook.h>

#include "Types.h"
#include "Main.h"

typedef unsigned long long uint64;

#endif //PCH_H
