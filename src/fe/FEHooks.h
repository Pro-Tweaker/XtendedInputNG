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

#pragma once

#ifndef NO_FENG
#ifndef GAME_MW
void HandleSelectCarCamera(void* SelectCarObj, SHORT XAxis, SHORT YAxis);

// fix for Carbon & Pro Street Photo Mode
void __stdcall FEPhotoModeStateManager_HandleScreenTick_Hook();

// fix for ALL zooming (ingame and FE)
void __stdcall FEPhotoModeStateManager_HandleLTrigger_Hook();
void __stdcall FEPhotoModeStateManager_HandleRTrigger_Hook();

#ifdef GAME_CARBON
int __stdcall ftol2_to_int_bool();
#endif
#endif  // GAME_MW

extern unsigned int GameWndProcAddr;
extern LRESULT(WINAPI* GameWndProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI CustomWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
ATOM WINAPI RegisterClassHook(WNDCLASSEXA* wcex);
void DummyFunc();
void __stdcall DummyFuncStd(BOOL dummy);
#endif  // NO_FENG

#ifdef GAME_UG2
extern bool bShowPadHints;
extern bool bShowKeyboardHints;
void        InstallUndergroundButtonHints();
#endif
