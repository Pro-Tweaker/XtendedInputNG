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
#include "game/Definitions.h"

#ifdef GAME_MW
unsigned int DebugWorldCameraMover_Update_Addr = 0x0047E7B0;

unsigned int FE_DefaultButtonTexHashes[FE_TEX_HASH_COUNT] = {0,
                                                             FE_ACCEPT_TEX_HASH,
                                                             FE_CANCEL_TEX_HASH,
                                                             FE_BUTTON4_TEX_HASH,
                                                             FE_BUTTON5_TEX_HASH,
                                                             FE_LTRIGGER_TEX_HASH,
                                                             FE_RTRIGGER_TEX_HASH,
                                                             FE_START_TEX_HASH,
                                                             FE_SELECT_TEX_HASH,
                                                             FE_BUTTON1_TEX_HASH,
                                                             FE_BUTTON2_TEX_HASH,
                                                             FE_BUTTON3_TEX_HASH,
                                                             FE_RANALOG_TEX_HASH,
                                                             FE_RANALOG_UP_TEX_HASH,
                                                             FE_RANALOG_UP_SWEDISH_TEX_HASH,
                                                             FE_RANALOG_UP_FRENCH_TEX_HASH,
                                                             FE_RANALOG_DOWN_TEX_HASH,
                                                             FE_RANALOG_DOWN_SWEDISH_TEX_HASH,
                                                             FE_RANALOG_DOWN_FRENCH_TEX_HASH,
                                                             FE_LANALOG_TEX_HASH,
                                                             FE_LANALOG_UP_TEX_HASH,
                                                             FE_LANALOG_UP_SWEDISH_TEX_HASH,
                                                             FE_LANALOG_UP_FRENCH_TEX_HASH,
                                                             FE_LANALOG_DOWN_TEX_HASH,
                                                             FE_LANALOG_DOWN_SWEDISH_TEX_HASH,
                                                             FE_LANALOG_DOWN_FRENCH_TEX_HASH,
                                                             FE_DPAD_TEX_HASH,
                                                             FE_DPAD_UPDOWN_TEX_HASH,
                                                             FE_DPAD_LEFTRIGHT_TEX_HASH,
                                                             FE_DPAD_UP_TEX_HASH,
                                                             FE_DPAD_DOWN_TEX_HASH,
                                                             FE_DPAD_LEFT_TEX_HASH,
                                                             FE_DPAD_RIGHT_TEX_HASH};

int (*FEPrintf)(const char* pkg_name, void* FEObject, const char* format, ...) = (int (*)(const char*, void*, const char*, ...))FEPRINTF_ADDRESS;
void* (*FEngFindObject_Title)(char* pkg_name, unsigned int obj_hash)           = (void* (*)(char*, unsigned int))FENG_FINDOBJECT_ADDR;
char* SplashPkgName   = "MW_LS_Splash.fng";
char* WSSplashPkgName = "WS_MW_LS_Splash.fng";

int TitleScreenVisible() {
  if ((*(int*)GAMEFLOWMANAGER_STATUS_ADDR == 3)) {
    if (cFEng_IsPackageInControl_Fast(SPLASH_FNG_HASH)) return 1;
    if (cFEng_IsPackageInControl_Fast(WS_SPLASH_FNG_HASH)) return 2;
  }
  return 0;
}

void SetTitleScreenText() {
  int   title_screen_type = TitleScreenVisible();
  int   obj_hash          = 0xC6CC55A4;
  char* pkg_name          = SplashPkgName;
  char* splash_txt        = FE_SPLASH_TEXT_PC;

  if (title_screen_type) {
    if (title_screen_type == 2) {
      obj_hash = 0xC4DF3FF2;
      pkg_name = WSSplashPkgName;
    }

    if (bConsoleFEng) {
      switch (ControllerIconMode) {
        case CONTROLLERICON_PS3:
          splash_txt = FE_SPLASH_TEXT_PS3;
          break;
        case CONTROLLERICON_XB360MW:
        case CONTROLLERICON_XBOX360:
          splash_txt = FE_SPLASH_TEXT_XBOX360;
          break;
        case CONTROLLERICON_PS4:
          splash_txt = FE_SPLASH_TEXT_PS4;
          break;
        case CONTROLLERICON_SWITCH:
          splash_txt = FE_SPLASH_TEXT_SWITCH;
          break;
        case CONTROLLERICON_XBOXONE:
        default:
          splash_txt = FE_SPLASH_TEXT_XBOX;
          break;
      }
    }

    FEPrintf(pkg_name, FEngFindObject_Title(pkg_name, obj_hash), splash_txt);
  }
}

#endif
#ifdef GAME_CARBON
unsigned int DebugWorldCameraMover_Update_Addr = 0x004932A0;

unsigned int FE_DefaultButtonTexHashes[FE_TEX_HASH_COUNT] = {
    0,
    FE_ACCEPT_TEX_HASH,
    FE_CANCEL_TEX_HASH,
    FE_BUTTON4_TEX_HASH,
    FE_BUTTON5_TEX_HASH,
    FE_LTRIGGER_TEX_HASH,
    FE_RTRIGGER_TEX_HASH,
    FE_BUTTON0_TEX_HASH,
    FE_START_TEX_HASH,
    FE_SELECT_TEX_HASH,
    FE_BUTTON1_TEX_HASH,
    FE_BUTTON2_TEX_HASH,
    FE_BUTTON3_TEX_HASH,
    FE_RANALOG_TEX_HASH,
    FE_RANALOG_UP_TEX_HASH,
    FE_RANALOG_UP_SWEDISH_TEX_HASH,
    FE_RANALOG_UP_FRENCH_TEX_HASH,
    FE_RANALOG_DOWN_TEX_HASH,
    FE_RANALOG_DOWN_SWEDISH_TEX_HASH,
    FE_RANALOG_DOWN_FRENCH_TEX_HASH,
    FE_LANALOG_TEX_HASH,
    FE_LANALOG_UP_TEX_HASH,
    FE_LANALOG_UP_SWEDISH_TEX_HASH,
    FE_LANALOG_UP_FRENCH_TEX_HASH,
    FE_LANALOG_DOWN_TEX_HASH,
    FE_LANALOG_DOWN_SWEDISH_TEX_HASH,
    FE_LANALOG_DOWN_FRENCH_TEX_HASH,
    FE_DPAD_TEX_HASH,
    FE_DPAD_UPDOWN_TEX_HASH,
    FE_DPAD_LEFTRIGHT_TEX_HASH,
    FE_DPAD_UP_TEX_HASH,
    FE_DPAD_DOWN_TEX_HASH,
    FE_DPAD_LEFT_TEX_HASH,
    FE_DPAD_RIGHT_TEX_HASH,
};

int (*FE_String_Printf)(void* FEObject, const char* fmt, ...)        = (int (*)(void*, const char*, ...))FE_STRING_PRINTF_ADDRESS;
void* (*FEngFindObject_Title)(char* pkg_name, unsigned int obj_hash) = (void* (*)(char*, unsigned int))FENG_FINDOBJECT_ADDR;
char* SplashPkgName = "DEMO_SPLASH.fng";

void SetTitleScreenText() {
  int   obj_hash   = 0xC4DF3FF2;
  char* pkg_name   = SplashPkgName;
  char* splash_txt = FE_SPLASH_TEXT_PC;

  if (bConsoleFEng) {
    switch (ControllerIconMode) {
      case CONTROLLERICON_PS3:
        splash_txt = FE_SPLASH_TEXT_PS3;
        break;
      case CONTROLLERICON_XB360MW:
      case CONTROLLERICON_XBOX360:
        splash_txt = FE_SPLASH_TEXT_XBOX360;
        break;
      case CONTROLLERICON_PS4:
        splash_txt = FE_SPLASH_TEXT_PS4;
        break;
      case CONTROLLERICON_SWITCH:
        splash_txt = FE_SPLASH_TEXT_SWITCH;
        break;
      case CONTROLLERICON_XBOXONE:
      default:
        splash_txt = FE_SPLASH_TEXT_XBOX;
        break;
    }
  }

  FE_String_Printf(FEngFindObject_Title(pkg_name, obj_hash), splash_txt);
}

void (*InitProfileSettings)() = (void (*)())0x00679BF0;

#endif
#ifdef GAME_PROSTREET
unsigned int DebugWorldCameraMover_Update_Addr = 0x00651BA0;

unsigned int FE_DefaultButtonTexHashes[FE_TEX_HASH_COUNT] = {
    0,
    FE_ACCEPT_TEX_HASH,
    FE_CANCEL_TEX_HASH,
    FE_BUTTON4_TEX_HASH,
    FE_BUTTON5_TEX_HASH,
    FE_LTRIGGER_TEX_HASH,
    FE_RTRIGGER_TEX_HASH,
    FE_BUTTON0_TEX_HASH,
    FE_START_TEX_HASH,
    FE_SELECT_TEX_HASH,
    FE_BUTTON1_TEX_HASH,
    FE_BUTTON2_TEX_HASH,
    FE_BUTTON3_TEX_HASH,
    FE_RANALOG_TEX_HASH,
    FE_RANALOG_UP_TEX_HASH,
    FE_RANALOG_UP_SWEDISH_TEX_HASH,
    FE_RANALOG_UP_FRENCH_TEX_HASH,
    FE_RANALOG_DOWN_TEX_HASH,
    FE_RANALOG_DOWN_SWEDISH_TEX_HASH,
    FE_RANALOG_DOWN_FRENCH_TEX_HASH,
    FE_LANALOG_TEX_HASH,
    FE_LANALOG_UP_TEX_HASH,
    FE_LANALOG_UP_SWEDISH_TEX_HASH,
    FE_LANALOG_UP_FRENCH_TEX_HASH,
    FE_LANALOG_DOWN_TEX_HASH,
    FE_LANALOG_DOWN_SWEDISH_TEX_HASH,
    FE_LANALOG_DOWN_FRENCH_TEX_HASH,
    FE_DPAD_TEX_HASH,
    FE_DPAD_UPDOWN_TEX_HASH,
    FE_DPAD_LEFTRIGHT_TEX_HASH,
    FE_DPAD_UP_TEX_HASH,
    FE_DPAD_DOWN_TEX_HASH,
    FE_DPAD_LEFT_TEX_HASH,
    FE_DPAD_RIGHT_TEX_HASH,
};

int (*FE_String_Printf)(void* FEObject, const char* fmt, ...)        = (int (*)(void*, const char*, ...))FE_STRING_PRINTF_ADDRESS;
void* (*FEngFindObject_Title)(char* pkg_name, unsigned int obj_hash) = (void* (*)(char*, unsigned int))FENG_FINDOBJECT_ADDR;
char* SplashPkgName = "Splash.fng";

void SetTitleScreenText() {
  int   obj_hash   = 0xC4DF3FF2;
  char* pkg_name   = SplashPkgName;
  char* splash_txt = FE_SPLASH_TEXT_PC;

  if (bConsoleFEng) {
    switch (ControllerIconMode) {
      case CONTROLLERICON_PS3:
        splash_txt = FE_SPLASH_TEXT_PS3;
        break;
      case CONTROLLERICON_XB360MW:
      case CONTROLLERICON_XBOX360:
        splash_txt = FE_SPLASH_TEXT_XBOX360;
        break;
      case CONTROLLERICON_PS4:
        splash_txt = FE_SPLASH_TEXT_PS4;
        break;
      case CONTROLLERICON_SWITCH:
        splash_txt = FE_SPLASH_TEXT_SWITCH;
        break;
      case CONTROLLERICON_XBOXONE:
      default:
        splash_txt = FE_SPLASH_TEXT_XBOX;
        break;
    }
  }

  FE_String_Printf(FEngFindObject_Title(pkg_name, obj_hash), splash_txt);
}

void (*InitProfileSettings)() = (void (*)())0x006A56C0;

#endif
#ifdef GAME_UC
float smallfloat = 0.000001;
void (*InitProfileSettings)() = (void (*)())0x006774C0;
unsigned int DebugWorldCameraMover_Update_Addr = 0x004E4040;

#endif
#ifdef GAME_WORLD
unsigned int MainBase;
float smallfloat = 0.000001;
unsigned int DebugWorldCameraMover_Update_Addr;
unsigned int DebugWorldCameraMover_Update_VT_Addr;

#endif
#ifdef GAME_UG2
void (*FESendKeystroke)(unsigned int key) = (void (*)(unsigned int))FESENDKEYSTROKE_ADDR;
#endif
