/*
// clang-format off
//
//    Xtended Input NG - native gamepad support for the PC versions of Need for Speed games
//    Continues xan1242's NFS-XtendedInput and NFSU-XtendedInput
//
//    MIT License
//
//    Copyright (c) 2026 Pro-Tweaker
//
// clang-format on
*/

#pragma once
#include <cstdint>

// Finds code in the running game's executable by bytes, so hooks survive on any build that has the same code:
// "E8 ? ? ? ? 8B 4E ?" (hex bytes, '?' = any byte). Returns the first match in the executable sections, or NULL.
uint8_t* FindPattern(const char* pattern);
