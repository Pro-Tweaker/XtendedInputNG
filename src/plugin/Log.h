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

// Writes scripts\XtendedInputNG.log next to the game. Opened (and truncated) by LogOpen, a no-op until then.
void LogOpen();
void Log(const char* fmt, ...);

// Warns in the log when UAC file virtualization is active for the game's folder: edits made to the INI files with
// admin rights would then not be what the game reads (see issue #71).
void LogVirtualizationWarning();
