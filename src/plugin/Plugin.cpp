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

unsigned int LastControlledDevice         = 0;  // 0 = keyboard, 1 = controller
unsigned int LastControlledDeviceOldState = 0;
unsigned int ControllerIconMode = 0;  // read modes above
bool         bAutoControllerIcons = false;
bool bInDebugWorldCamera   = false;
bool bGlobalDoPolling      = true;
bool bBlockThisPoll        = false;
bool bEnableSplashTakeover = true;
bool bConsoleFEng          = false;
DWORD gChaseMouseLookTick = 0;
