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
#include "input/Mapping.h"
#define MINI_CASE_SENSITIVE
#include "plugin/Plugin.h"
#include "game/ActionID.h"
#include "input/XInputConfig.h"
#include "input/VKHash.h"
#include "input/Controller.h"
#include "fe/FEng.h"
#include <mINI/src/mini/ini.h>

#ifndef GAME_UNDERGROUND

#pragma runtime_checks("", off)

std::string gProfileName;

// profile names come from the save file, keep them from escaping the mapping directory
static std::string SanitizeProfileName(const std::string& name) {
  std::string out = name;
  for (char& c : out) {
    if (c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|' || (unsigned char)c < 0x20) c = '_';
  }
  if (!out.empty() && out.find_first_not_of('.') == std::string::npos) out = "_";
  return out;
}

static std::string UserMappingFilePath(const std::string& profile) {
  std::string path = userMappingDir;
  path += '\\';
  path += SanitizeProfileName(profile);
  path += '\\';
  path += userMappingName;
  return path;
}

// returns the profile's mapping file, creating it from the default one if needed. falls back to the default file on failure
static std::string ResolveMappingPath(const std::string& profile) {
  if (profile.empty()) return defMappingName;

  std::string mapFile = UserMappingFilePath(profile);
  if (PathFileExists(mapFile.c_str())) return mapFile;

  std::string dirName = userMappingDir;
  if (!PathFileExists(dirName.c_str()) && !CreateDirectory(dirName.c_str(), NULL)) return defMappingName;
  dirName += '\\';
  dirName += SanitizeProfileName(profile);
  if (!PathFileExists(dirName.c_str()) && !CreateDirectory(dirName.c_str(), NULL)) return defMappingName;
  if (!CopyFile(defMappingName, mapFile.c_str(), TRUE)) return defMappingName;
  return mapFile;
}

#ifdef GAME_MW
std::string oldProfileName;
uintptr_t   MemCardLoadAddr = 0x005189F0;

void __stdcall MemoryCard_Load_Hook(const char* profileName) {
  uintptr_t that;
  _asm mov  that, ecx;
  reinterpret_cast<void(__thiscall*)(uintptr_t, const char*)>(MemCardLoadAddr)(that, profileName);

  gProfileName = profileName;
  if (gProfileName.compare(oldProfileName) || oldProfileName.empty()) {
    oldProfileName = gProfileName;
    LoadMapping(gProfileName);
  }
}

#endif
#if defined(GAME_CARBON) || defined(GAME_PROSTREET)
char        NFSProfileName[128];
uintptr_t DALManager_GetString_Addr = DALMANAGER_GETSTRING_ADDR;
uintptr_t   gDALManager_Addr          = DALMANAGER_ADDR;

bool GetNFSProfileName() {
    // get it via DALManager
  return reinterpret_cast<bool(__thiscall*)(uintptr_t DALManager, const int valueType, char* getVal, const int getLen, const int arg1, const int arg2,
                                     const int arg3)>(DALManager_GetString_Addr)(gDALManager_Addr, DALMANAGER_PROFILENAME_TYPEVAL, NFSProfileName, 128, -1, -1, -1);
}

#endif
#ifdef GAME_CARBON
uintptr_t OnLoadDoneFunc = 0x0058E250;

void __stdcall OnLoadDoneHook(const char* str) {
  uintptr_t that;
  _asm mov  that, ecx;
  reinterpret_cast<void(__thiscall*)(uintptr_t, const char*)>(OnLoadDoneFunc)(that, str);
  GetNFSProfileName();
  if (gProfileName.compare(NFSProfileName) || gProfileName.empty()) {
    gProfileName = NFSProfileName;
    LoadMapping(gProfileName);
  }
}

#endif
#ifdef GAME_PROSTREET
uintptr_t OnLoadDoneFunc = 0x00543CC0;

void __stdcall OnLoadDoneHook() {
  uintptr_t that;
  _asm mov that, ecx;
  reinterpret_cast<void(__thiscall*)(uintptr_t)>(OnLoadDoneFunc)(that);
  GetNFSProfileName();
  if (gProfileName.compare(NFSProfileName) || gProfileName.empty()) {
    gProfileName = NFSProfileName;
    LoadMapping(gProfileName);
  }
}

#endif
// button bindings per action ID
WORD XInputBindings_PRIMARY[MAX_ACTIONID];
WORD XInputBindings_SECONDARY[MAX_ACTIONID];

// secondary bindings, exclusively for FE navigation with left stick as well (not just D-Pad)
WORD FE_Secondary_Up;
WORD FE_Secondary_Down;
WORD FE_Secondary_Left;
WORD FE_Secondary_Right;
BYTE VKeyBindings_PRIMARY[MAX_ACTIONID];
BYTE VKeyBindings_SECONDARY[MAX_ACTIONID];

#ifdef GAME_PROSTREET
bool     g_bRemapClear           = false;
uint32_t g_bRemapClear_IsPrimary = std::numeric_limits<std::uint32_t>::max();
uint32_t g_iRemapClear_BtnIndex  = std::numeric_limits<std::uint32_t>::max();

#endif
#ifdef GAME_PROSTREET
bool bIgnorableVKey(int k) {
  if (bIsReservedVK(k)) return true;
  switch (k) {
    case VK_MENU:
    case VK_CONTROL:
    case VK_SHIFT:
    case VK_ESCAPE:
    case VK_LWIN:
    case VK_RWIN:
    case VK_APPS:
    case VK_LBUTTON:
    case VK_RBUTTON:
    case VK_MBUTTON:
    case VK_XBUTTON1:
    case VK_XBUTTON2:
      return true;
    default:
      return false;
  }

  return false;
}

int KB_GetCurrentPressedKey() {
  switch (KeyboardReadingMode) {
    case KB_READINGMODE_BUFFERED:

      for (int i = 1; i < 255; i++) {
        if (i && VKeyStates[0][i] >> 7)
          if (!bIgnorableVKey(i))  // TODO: separate Left Control and Right Alt
            return i;
      }
      return -1;
    case KB_READINGMODE_UNBUFFERED_ASYNC:
    default:

      for (int i = 1; i < 255; i++) {
        if (i && GetAsyncKeyState(i) >> 15)
          if (!bIgnorableVKey(i)) return i;
      }
      return -1;
  }
}

void SaveBindingToIni(ActionID id, bool isPrimary, uint16_t bind) {
  std::string mapFile = ResolveMappingPath(gProfileName);

  mINI::INIFile      inifile(mapFile);
  mINI::INIStructure ini;
  inifile.read(ini);

  if (bind == 0) {
    if (ini.has(isPrimary ? "Events" : "Events_Secondary")) {
      ini[isPrimary ? "Events" : "Events_Secondary"][ActionIDStr[id]] = "";
      inifile.write(ini, true);
    }
    return;
  }

  ini[isPrimary ? "Events" : "Events_Secondary"][ActionIDStr[id]] = ConvertXInputBitmaskToName(bind);
  inifile.write(ini, true);
}

void SaveBindingToIniKB(ActionID id, bool isPrimary, int bind) {
  std::string mapFile = ResolveMappingPath(gProfileName);

  mINI::INIFile      inifile(mapFile);
  mINI::INIStructure ini;
  inifile.read(ini);

  if (bind == 0) {
    if (ini.has(isPrimary ? "EventsKB" : "EventsKB_Secondary")) {
      ini[isPrimary ? "EventsKB" : "EventsKB_Secondary"][ActionIDStr[id]] = "";
      inifile.write(ini, true);
    }
    return;
  }

  ini[isPrimary ? "EventsKB" : "EventsKB_Secondary"][ActionIDStr[id]] = VKeyStrings[bind];
  inifile.write(ini, true);
}

void ClearExistingMapKB(int bind) {
  for (int i = 0; i < MAX_ACTIONID; i++) {
    if (bActionUserRemappable((ActionID)i)) {
      if (VKeyBindings_PRIMARY[i] == bind) {
        VKeyBindings_PRIMARY[i] = 0;
        SaveBindingToIniKB((ActionID)i, true, 0);
      }
      if (VKeyBindings_SECONDARY[i] == bind) {
        VKeyBindings_SECONDARY[i] = 0;
        SaveBindingToIniKB((ActionID)i, false, 0);
      }
    }
  }
}

void ClearExistingMap(uint16_t bind) {
  for (int i = 0; i < MAX_ACTIONID; i++) {
    if (bActionUserRemappable((ActionID)i)) {
      if (XInputBindings_PRIMARY[i] == bind) {
        XInputBindings_PRIMARY[i] = 0;
        SaveBindingToIni((ActionID)i, true, 0);
      }
      if (XInputBindings_SECONDARY[i] == bind) {
        XInputBindings_SECONDARY[i] = 0;
        SaveBindingToIni((ActionID)i, false, 0);
      }
    }
  }
}

bool HandleKeyboardRemap(uint32_t index, uint32_t isPrimary) {
  int      KBkey = g_bRemapClear ? 0 : KB_GetCurrentPressedKey();
  ActionID id    = FindPCRemapActionID(index);
  if (KBkey != -1) {
    ClearExistingMapKB(KBkey);
    if (isPrimary)
      VKeyBindings_PRIMARY[id] = KBkey;
    else
      VKeyBindings_SECONDARY[id] = KBkey;
    SaveBindingToIniKB(id, isPrimary, KBkey);
    return true;
  }
  return false;
}

bool HandleGamepadRemap(uint32_t index, uint32_t isPrimary, int fDeviceIndex) {
  ActionID id = FindPCRemapActionID(index);
  if (g_bRemapClear) {
    if (isPrimary)
      XInputBindings_PRIMARY[id] = 0;
    else
      XInputBindings_SECONDARY[id] = 0;
    SaveBindingToIni(id, isPrimary, 0);
    if (bIsActionTextureBindable(id)) SetBindingButtonTexture(id, isPrimary ? XInputBindings_PRIMARY[id] : XInputBindings_SECONDARY[id]);
    return true;
  }

  uint16_t act = GetAnalogActivity();
  if (act) {
    ClearExistingMap(act);
    if (isPrimary)
      XInputBindings_PRIMARY[id] = act;
    else
      XInputBindings_SECONDARY[id] = act;
    SaveBindingToIni(id, isPrimary, act);
    if (bIsActionTextureBindable(id)) SetBindingButtonTexture(id, isPrimary ? XInputBindings_PRIMARY[id] : XInputBindings_SECONDARY[id]);
    return true;
  }

  WORD buttons = g_Controllers[fDeviceIndex].state.Gamepad.wButtons;
  if (buttons) {
    ClearExistingMap(buttons);
    if (isPrimary)
      XInputBindings_PRIMARY[id] = buttons;
    else
      XInputBindings_SECONDARY[id] = buttons;
    SaveBindingToIni(id, isPrimary, buttons);
    if (bIsActionTextureBindable(id)) SetBindingButtonTexture(id, isPrimary ? XInputBindings_PRIMARY[id] : XInputBindings_SECONDARY[id]);
    return true;
  }

  return false;
}

void __stdcall ResetMappingsToDefault() {
  void*    that;
  _asm mov that, ecx

  // LoadMapping recreates the profile's file from the default one
  if (!gProfileName.empty()) DeleteFile(UserMappingFilePath(gProfileName).c_str());
  ClearAllMappings();
  LoadMapping(gProfileName);

  //for (unsigned int i = 0; i < MAX_ACTIONID; i++) {
  //  SaveBindingToIni((ActionID)i, true, XInputBindings_PRIMARY[i]);
  //  SaveBindingToIni((ActionID)i, false, XInputBindings_SECONDARY[i]);
  //  SaveBindingToIniKB((ActionID)i, true, VKeyBindings_PRIMARY[i]);
  //  SaveBindingToIniKB((ActionID)i, false, VKeyBindings_SECONDARY[i]);
  //}

  reinterpret_cast<void(__thiscall*)(void*)>(0x0070BDE0)(that);
}

#endif
void ClearAllMappings() {
  for (unsigned int i = 0; i < MAX_ACTIONID; i++) {
    VKeyBindings_PRIMARY[i] = 0;
    VKeyBindings_SECONDARY[i] = 0;
    XInputBindings_PRIMARY[i] = 0;
    XInputBindings_SECONDARY[i] = 0;
  }
}

// fetches a binding value without inserting empty keys into the INI
static std::string GetIniValue(mINI::INIStructure& ini, const char* section, const char* key) {
  if (!ini.has(section)) return std::string();
  return ini[section].get(key);
}

static WORD ParseXInputBinding(const std::string& value) {
  int def = ConvertXInputOtherConfigDef(value);
  return def ? (WORD)def : ConvertXInputNameToBitmask(value);
}

static BYTE ParseKeyBinding(const std::string& value) {
  BYTE vkey = ConvertVKNameToValue(value);
  // try checking for single-char
  if (vkey == 0 && value.size() == 1) vkey = (BYTE)::toupper(value.at(0));
  if (bIsReservedVK(vkey)) vkey = 0;  // e.g. RESERVED_0X7
  return vkey;
}

void LoadMapping(std::string userProfileName) {
  std::string mapFile = ResolveMappingPath(userProfileName);

  mINI::INIFile      inifile(mapFile);
  mINI::INIStructure ini;
  inifile.read(ini);

  for (unsigned int i = 0; i < MAX_ACTIONID; i++) {
    const char* action = ActionIDStr[i];

    // read the key bindings
    if (ini.has("EventsKB") && ini["EventsKB"].has(action)) VKeyBindings_PRIMARY[i] = ParseKeyBinding(ini["EventsKB"].get(action));

    // read the key bindings [SECONDARY]
    if (ini.has("EventsKB_Secondary") && ini["EventsKB_Secondary"].has(action)) VKeyBindings_SECONDARY[i] = ParseKeyBinding(ini["EventsKB_Secondary"].get(action));

    if (ini.has("Events")) {
      XInputBindings_PRIMARY[i] = ParseXInputBinding(ini["Events"].get(action));
#ifndef NO_FENG
      if (bIsActionTextureBindable((ActionID)i)) SetBindingButtonTexture((ActionID)i, XInputBindings_PRIMARY[i]);
#endif
    }
    if (ini.has("Events_Secondary")) XInputBindings_SECONDARY[i] = ParseXInputBinding(ini["Events_Secondary"].get(action));
  }

  if (ini.has("Events")) {
    FE_Secondary_Up    = ParseXInputBinding(GetIniValue(ini, "Events", FE_SECONDARY_UP_NAME));
    FE_Secondary_Down  = ParseXInputBinding(GetIniValue(ini, "Events", FE_SECONDARY_DOWN_NAME));
    FE_Secondary_Left  = ParseXInputBinding(GetIniValue(ini, "Events", FE_SECONDARY_LEFT_NAME));
    FE_Secondary_Right = ParseXInputBinding(GetIniValue(ini, "Events", FE_SECONDARY_RIGHT_NAME));
  }
}

#pragma runtime_checks("", restore)

#endif  // !GAME_UNDERGROUND
