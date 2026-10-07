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

#include "stdafx.h"
#include "plugin/Plugin.h"
#include "fe/FEng.h"

extern "C" __declspec(dllexport) bool GetPollingState() { return bGlobalDoPolling; }

extern "C" __declspec(dllexport) bool SetPollingState(bool state) {
  bGlobalDoPolling = state;
  return state;
}

// Blocks a poll for exactly 1 poll cycle -- useful for frame-accurate blocks
extern "C" __declspec(dllexport) void BlockNextPoll() {
  bBlockThisPoll = true;
}

extern "C" __declspec(dllexport) bool GetBlockNextPoll() { return bBlockThisPoll; }

#ifndef NO_FENG
extern "C" __declspec(dllexport) float GetFEScale() { return fFEScale; }

extern "C" __declspec(dllexport) float SetFEScale(float val) {
  fFEScale = val;
  return fFEScale;
}

extern "C" __declspec(dllexport) bool GetUseWin32Cursor() { return bUseWin32Cursor; }

#endif
