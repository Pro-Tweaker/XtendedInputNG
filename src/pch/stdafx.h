/*
// clang-format off
//
//    Xtended Input NG - native gamepad support for the PC versions of Need for Speed games
//    Continues xan1242's NFS-XtendedInput and NFSU-XtendedInput
//
//    MIT License
//
//    Copyright (c) 2022–2023 Lovro Pleše
//    Copyright (c) 2023 Berkay Yiğit <mail@berkay.link>
//    Copyright (c) 2026 Pro-Tweaker
//
// clang-format on
*/

#ifndef XTENDEDINPUTNG_PCH_H
#define XTENDEDINPUTNG_PCH_H
#pragma once

#include "macros.h"
MSVC_WARNINGS_OFF

// Win32 targeting
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00  // _WIN32_WINNT_WINTHRESHOLD
#include <SDKDDKVer.h>
// Win32
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

MSVC_WARNINGS_RESTORE

// C/C++ standard library
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

// Per-game feature macros (GAME_*, NO_FENG, ...) and addresses must be visible before anything else
#include "game/Definitions.h"

inline float clampf(const float& val, const float& min, const float& max) { return val < min ? min : val > max ? max : val; }

#endif