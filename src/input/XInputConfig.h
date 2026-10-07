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
#include "game/ActionID.h"
#ifdef GAME_PROSTREET
#include <vector>
//using namespace std;
#include <string>
#include <algorithm>
#endif

int bStringHash(char* a1);

//
// Constants for gamepad buttons -- from XInput.h
//
#define XINPUT_GAMEPAD_DPAD_UP        0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN      0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT      0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT     0x0008
#define XINPUT_GAMEPAD_START          0x0010
#define XINPUT_GAMEPAD_BACK           0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB     0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB    0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER  0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER 0x0200
#define XINPUT_GAMEPAD_A              0x1000
#define XINPUT_GAMEPAD_B              0x2000
#define XINPUT_GAMEPAD_X              0x4000
#define XINPUT_GAMEPAD_Y              0x8000

// other gamepad stuff -- sticks & triggers
#define XINPUT_GAMEPAD_LT_CONFIGDEF       0xABC0
#define XINPUT_GAMEPAD_RT_CONFIGDEF       0xABC1
#define XINPUT_GAMEPAD_LS_X_CONFIGDEF     0xABC2
#define XINPUT_GAMEPAD_LS_Y_CONFIGDEF     0xABC3
#define XINPUT_GAMEPAD_RS_X_CONFIGDEF     0xABC4
#define XINPUT_GAMEPAD_RS_Y_CONFIGDEF     0xABC5
#define XINPUT_GAMEPAD_LS_UP_CONFIGDEF    0xABC7
#define XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF  0xABC8
#define XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF  0xABC9
#define XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF 0xABCA
#define XINPUT_GAMEPAD_RS_UP_CONFIGDEF    0xABCB
#define XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF  0xABCC
#define XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF  0xABCD
#define XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF 0xABCE

#define XINPUT_GAMEPAD_DPAD_CONFIGDEF 0xDAD0  // only for internal use
#define XINPUT_GAMEPAD_LS_CONFIGDEF   0xDAD1  // only for internal use
#define XINPUT_GAMEPAD_RS_CONFIGDEF   0xDAD2  // only for internal use

#define XINPUT_GAMEPAD_DPAD_UPDOWN_CONFIGDEF    0xDAD3  // only for internal use
#define XINPUT_GAMEPAD_DPAD_LEFTRIGHT_CONFIGDEF 0xDAD4  // only for internal use

// comparing strings by hash
#define XINPUT_GAMEPAD_DPAD_UP_HASH        NFS_HASH("XINPUT_GAMEPAD_DPAD_UP")
#define XINPUT_GAMEPAD_DPAD_DOWN_HASH      NFS_HASH("XINPUT_GAMEPAD_DPAD_DOWN")
#define XINPUT_GAMEPAD_DPAD_LEFT_HASH      NFS_HASH("XINPUT_GAMEPAD_DPAD_LEFT")
#define XINPUT_GAMEPAD_DPAD_RIGHT_HASH     NFS_HASH("XINPUT_GAMEPAD_DPAD_RIGHT")
#define XINPUT_GAMEPAD_START_HASH          NFS_HASH("XINPUT_GAMEPAD_START")
#define XINPUT_GAMEPAD_BACK_HASH           NFS_HASH("XINPUT_GAMEPAD_BACK")
#define XINPUT_GAMEPAD_LEFT_THUMB_HASH     NFS_HASH("XINPUT_GAMEPAD_LEFT_THUMB")
#define XINPUT_GAMEPAD_RIGHT_THUMB_HASH    NFS_HASH("XINPUT_GAMEPAD_RIGHT_THUMB")
#define XINPUT_GAMEPAD_LSB_HASH            NFS_HASH("XINPUT_GAMEPAD_LSB")
#define XINPUT_GAMEPAD_RSB_HASH            NFS_HASH("XINPUT_GAMEPAD_RSB")
#define XINPUT_GAMEPAD_L3_HASH             NFS_HASH("XINPUT_GAMEPAD_L3")
#define XINPUT_GAMEPAD_R3_HASH             NFS_HASH("XINPUT_GAMEPAD_R3")
#define XINPUT_GAMEPAD_LS_CLICK_HASH       NFS_HASH("XINPUT_GAMEPAD_LS_CLICK")
#define XINPUT_GAMEPAD_RS_CLICK_HASH       NFS_HASH("XINPUT_GAMEPAD_RS_CLICK")
#define XINPUT_GAMEPAD_LEFT_SHOULDER_HASH  NFS_HASH("XINPUT_GAMEPAD_LEFT_SHOULDER")
#define XINPUT_GAMEPAD_RIGHT_SHOULDER_HASH NFS_HASH("XINPUT_GAMEPAD_RIGHT_SHOULDER")
#define XINPUT_GAMEPAD_LB_HASH             NFS_HASH("XINPUT_GAMEPAD_LB")
#define XINPUT_GAMEPAD_RB_HASH             NFS_HASH("XINPUT_GAMEPAD_RB")
#define XINPUT_GAMEPAD_A_HASH              NFS_HASH("XINPUT_GAMEPAD_A")
#define XINPUT_GAMEPAD_B_HASH              NFS_HASH("XINPUT_GAMEPAD_B")
#define XINPUT_GAMEPAD_X_HASH              NFS_HASH("XINPUT_GAMEPAD_X")
#define XINPUT_GAMEPAD_Y_HASH              NFS_HASH("XINPUT_GAMEPAD_Y")
#define XINPUT_GAMEPAD_LT_HASH             NFS_HASH("XINPUT_GAMEPAD_LT")
#define XINPUT_GAMEPAD_RT_HASH             NFS_HASH("XINPUT_GAMEPAD_RT")
#define XINPUT_GAMEPAD_LS_X_HASH           NFS_HASH("XINPUT_GAMEPAD_LS_X")
#define XINPUT_GAMEPAD_LS_Y_HASH           NFS_HASH("XINPUT_GAMEPAD_LS_Y")
#define XINPUT_GAMEPAD_RS_X_HASH           NFS_HASH("XINPUT_GAMEPAD_RS_X")
#define XINPUT_GAMEPAD_RS_Y_HASH           NFS_HASH("XINPUT_GAMEPAD_RS_Y")
#define XINPUT_GAMEPAD_DPAD_HASH           NFS_HASH("XINPUT_GAMEPAD_DPAD")
#define XINPUT_GAMEPAD_LS_UP_HASH          NFS_HASH("XINPUT_GAMEPAD_LS_UP")
#define XINPUT_GAMEPAD_LS_DOWN_HASH        NFS_HASH("XINPUT_GAMEPAD_LS_DOWN")
#define XINPUT_GAMEPAD_LS_LEFT_HASH        NFS_HASH("XINPUT_GAMEPAD_LS_LEFT")
#define XINPUT_GAMEPAD_LS_RIGHT_HASH       NFS_HASH("XINPUT_GAMEPAD_LS_RIGHT")
#define XINPUT_GAMEPAD_RS_UP_HASH          NFS_HASH("XINPUT_GAMEPAD_RS_UP")
#define XINPUT_GAMEPAD_RS_DOWN_HASH        NFS_HASH("XINPUT_GAMEPAD_RS_DOWN")
#define XINPUT_GAMEPAD_RS_LEFT_HASH        NFS_HASH("XINPUT_GAMEPAD_RS_LEFT")
#define XINPUT_GAMEPAD_RS_RIGHT_HASH       NFS_HASH("XINPUT_GAMEPAD_RS_RIGHT")

WORD ConvertXInputNameToBitmask(std::string name);
int ConvertXInputOtherConfigDef(std::string name);
bool bIsBindingAnalog(WORD in);
int IsBindingNavigation(WORD in);

#ifdef GAME_PROSTREET
const char* ConvertXInputBitmaskToName(uint16_t in);
extern const char* unboundstr;
extern const char* ControlsTextsXBOX[];
extern const char* ControlsTextsPS[];
extern const char* ControlsTextsPS3[];
extern const char* ControlsTextsNSW[];
extern const char* ControlsTextsPC[];
extern const size_t ControlsTextsPC_Count;
extern const char* VKeyStrings[];
const char* __stdcall ConvertBitmaskToControlString_XBOX(uint16_t in);
const char* __stdcall ConvertBitmaskToControlString_PS(uint16_t in);
const char* __stdcall ConvertBitmaskToControlString_Step2(uint16_t in, const char** strs);
const char* __stdcall ConvertBitmaskToControlString(int mode, uint16_t in);
extern std::vector<ActionID> PCRemapActionList;
ActionID FindPCRemapActionID(uint32_t idx);
bool bActionUserRemappable(ActionID id);

#endif
