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

#ifndef GAME_UNDERGROUND
#include "game/ActionID.h"

void        LoadMapping(std::string userProfileName);
void        ClearAllMappings();
extern std::string gProfileName;

#ifdef GAME_MW
extern std::string oldProfileName;
extern uintptr_t MemCardLoadAddr;
void __stdcall MemoryCard_Load_Hook(const char* profileName);

#endif
#if defined(GAME_CARBON) || defined(GAME_PROSTREET)
extern char NFSProfileName[128];
extern uintptr_t DALManager_GetString_Addr;
extern uintptr_t gDALManager_Addr;
bool GetNFSProfileName();

#endif
#if defined(GAME_UC) || defined(GAME_WORLD)
constexpr const char* NFSProfileName = "UserProfile";

#endif
#ifdef GAME_CARBON
extern uintptr_t OnLoadDoneFunc;
void __stdcall OnLoadDoneHook(const char* str);

#endif
#ifdef GAME_PROSTREET
extern uintptr_t OnLoadDoneFunc;
void __stdcall OnLoadDoneHook();

#endif
// button bindings per action ID
extern WORD XInputBindings_PRIMARY[MAX_ACTIONID];
extern WORD XInputBindings_SECONDARY[MAX_ACTIONID];

// secondary bindings, exclusively for FE navigation with left stick as well (not just D-Pad)
extern WORD FE_Secondary_Up;
extern WORD FE_Secondary_Down;
extern WORD FE_Secondary_Left;
extern WORD FE_Secondary_Right;
extern BYTE VKeyBindings_PRIMARY[MAX_ACTIONID];
extern BYTE VKeyBindings_SECONDARY[MAX_ACTIONID];

#ifdef GAME_PROSTREET
extern bool g_bRemapClear;
extern uint32_t g_bRemapClear_IsPrimary;
extern uint32_t g_iRemapClear_BtnIndex;

#endif
#ifdef GAME_PROSTREET
bool bIgnorableVKey(int k);
int KB_GetCurrentPressedKey();
void SaveBindingToIni(ActionID id, bool isPrimary, uint16_t bind);
void SaveBindingToIniKB(ActionID id, bool isPrimary, int bind);
void ClearExistingMapKB(int bind);
void ClearExistingMap(uint16_t bind);
bool HandleKeyboardRemap(uint32_t index, uint32_t isPrimary);
bool HandleGamepadRemap(uint32_t index, uint32_t isPrimary, int fDeviceIndex);
void __stdcall ResetMappingsToDefault();

#endif
void ClearAllMappings();
void LoadMapping(std::string userProfileName);

#endif  // !GAME_UNDERGROUND
