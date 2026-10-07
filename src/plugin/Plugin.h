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

template <unsigned int V>
static constexpr unsigned int compiler_stringhash_template = V;
static constexpr unsigned int compiler_stringhash(char* str) {
  if (str == NULL) return 0;

  char*        _str   = ((char*)str);
  unsigned int result = -1;

  while (*_str != 0) {
    result = result * 0x21 + (unsigned int)(*_str);
    _str   = _str + 1;
  }

  return result;
}

#define NFS_HASH(s) compiler_stringhash_template<compiler_stringhash(s)>

#define FE_SPLASH_TEXT_XBOX    "Press MENU or A button"
#define FE_SPLASH_TEXT_XBOX360 "Press START or A button"
#define FE_SPLASH_TEXT_PS4     "Press OPTIONS or X button"
#define FE_SPLASH_TEXT_PS3     "Press START or X button"
#define FE_SPLASH_TEXT_SWITCH  "Press + button"
#define FE_SPLASH_TEXT_PC      "CLICK or ENTER to continue"

#define LASTCONTROLLED_KB         0
#define LASTCONTROLLED_CONTROLLER 1
extern unsigned int LastControlledDevice;
extern unsigned int LastControlledDeviceOldState;

#define CONTROLLERICON_XBOXONE 0
#define CONTROLLERICON_PS4     1
#define CONTROLLERICON_XBOX360 2
#define CONTROLLERICON_PS3     3
#define CONTROLLERICON_SWITCH  4
#define CONTROLLERICON_XB360MW 5

extern unsigned int ControllerIconMode;
extern bool         bAutoControllerIcons;  // ControllerIconMode = -1: pick the icon set from the connected pad (SDL backend)
extern bool bInDebugWorldCamera;
extern bool bGlobalDoPolling;
extern bool bBlockThisPoll;
extern bool bEnableSplashTakeover;
extern bool bConsoleFEng;
extern DWORD gChaseMouseLookTick;  // GetTickCount() of the last frame the chase camera used the mouse (Carbon)
constexpr const char* cfgIniName      = "scripts\\XtendedInputNG.ini";
constexpr const char* defMappingName  = "scripts\\XtendedInputNG.default.ini";
constexpr const char* userMappingName = "XtendedInputNG.usermap.ini";
constexpr const char* userMappingDir  = "scripts\\XtendedInputNGMaps";
constexpr const char* cursorSkinPath  = "scripts\\nfs_cursor.cur";

#if defined(GAME_MW)
#define GAME_NAME_STR "Most Wanted"
#elif defined(GAME_CARBON)
#define GAME_NAME_STR "Carbon"
#elif defined(GAME_PROSTREET)
#define GAME_NAME_STR "ProStreet"
#elif defined(GAME_UC)
#define GAME_NAME_STR "Undercover"
#elif defined(GAME_WORLD)
#define GAME_NAME_STR "World"
#elif defined(GAME_UG)
#define GAME_NAME_STR "Underground"
#elif defined(GAME_UG2)
#define GAME_NAME_STR "Underground 2"
#endif
