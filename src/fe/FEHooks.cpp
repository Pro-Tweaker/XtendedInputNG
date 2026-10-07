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
#include "fe/FEHooks.h"
#include "plugin/Plugin.h"
#include "input/Controller.h"
#include "fe/FEng.h"

#pragma runtime_checks("", off)

#ifdef GAME_MW
#else
#ifndef NO_FENG
void HandleSelectCarCamera(void* SelectCarObj, SHORT XAxis, SHORT YAxis) {
  if (XAxis || YAxis || bMousePressedDown) {
    if (bMousePressedDown) {
      float HSpeed = *(int*)FEMOUSECURSOR_CARORBIT_X_ADDR;
      float VSpeed = *(int*)FEMOUSECURSOR_CARORBIT_Y_ADDR;

      if (HSpeed > 25.0) HSpeed = 25.0;
      if (HSpeed < -25.0) HSpeed = -25.0;
      if (VSpeed > 25.0) VSpeed = 25.0;
      if (VSpeed < -25.0) VSpeed = -25.0;

      if (HSpeed)
        SelectCarCameraMover_SetHRotateSpeed(SelectCarObj, HSpeed, true);
      else
        SelectCarCameraMover_SetHRotateSpeed(SelectCarObj, 0, false);
      if (VSpeed)
        SelectCarCameraMover_SetVRotateSpeed(SelectCarObj, VSpeed, true);
      else
        SelectCarCameraMover_SetVRotateSpeed(SelectCarObj, 0, false);
    }

    if (XAxis) {
      float HSpeed = -XAxis / float(0x7FFF);
      SelectCarCameraMover_SetHRotateSpeed(SelectCarObj, HSpeed, true);
    }
    if (YAxis) {
      float VSpeed = YAxis / float(0x7FFF);
      SelectCarCameraMover_SetVRotateSpeed(SelectCarObj, VSpeed, true);
    }
  } else {
    SelectCarCameraMover_SetHRotateSpeed(SelectCarObj, 0, false);
    SelectCarCameraMover_SetVRotateSpeed(SelectCarObj, 0, false);
  }

  if (g_Controllers[0].state.Gamepad.bLeftTrigger || (MouseWheelValue < 0)) {
    if (MouseWheelValue)  // TODO: maybe enhance it with a proper accel delta???
      SelectCarCameraMover_SetZoomSpeed(SelectCarObj, 1.0, true);
    if (g_Controllers[0].state.Gamepad.bLeftTrigger) {
      float Speed = g_Controllers[0].state.Gamepad.bLeftTrigger / float(0xFF);
      SelectCarCameraMover_SetZoomSpeed(SelectCarObj, Speed, true);
    }
  }

  else if (g_Controllers[0].state.Gamepad.bRightTrigger || (MouseWheelValue > 0)) {
    if (MouseWheelValue) SelectCarCameraMover_SetZoomSpeed(SelectCarObj, -1.0, true);
    if (g_Controllers[0].state.Gamepad.bRightTrigger) {
      float Speed = g_Controllers[0].state.Gamepad.bRightTrigger / float(0xFF);
      SelectCarCameraMover_SetZoomSpeed(SelectCarObj, -Speed, true);
    }
  } else
    SelectCarCameraMover_SetZoomSpeed(SelectCarObj, 0, false);
}

// fix for Carbon & Pro Street Photo Mode
void __stdcall FEPhotoModeStateManager_HandleScreenTick_Hook() {
  unsigned int thethis = 0;
  _asm mov thethis, ecx;
  void* SelectCarObj = (void*)*(int*)(thethis + SELECTCAROBJ_OFFSET);
#ifdef GAME_PROSTREET
  HandleSelectCarCamera(SelectCarObj, g_Controllers[0].state.Gamepad.sThumbLX, g_Controllers[0].state.Gamepad.sThumbLY);
#else
  HandleSelectCarCamera(SelectCarObj, g_Controllers[0].state.Gamepad.sThumbRX, g_Controllers[0].state.Gamepad.sThumbRY);
#endif
}

// fix for ALL zooming (ingame and FE)
void __stdcall FEPhotoModeStateManager_HandleLTrigger_Hook() { return; }

void __stdcall FEPhotoModeStateManager_HandleRTrigger_Hook() { return; }

#ifdef GAME_CARBON
int __stdcall ftol2_to_int_bool() {
  unsigned int obj = 0;
  _asm mov obj, ecx

      if (obj) {
    float num = *(float*)(obj + 8);
    if (num) return 1;
  }
  return 0;
}

#endif
#endif  // GAME_UC
#endif
#ifndef NO_FENG
unsigned int GameWndProcAddr = 0;
LRESULT(WINAPI* GameWndProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI CustomWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_LBUTTONDOWN:
      LastControlledDevice = LASTCONTROLLED_KB;
      bMousePressedDown    = true;
#ifndef NO_FENG
      if (bUseDynamicFEngSwitching) bConsoleFEng = false;
#endif
      return 0;
    case WM_LBUTTONUP:
      bMousePressedDown = false;
      return 0;
    case WM_RBUTTONDOWN:
      LastControlledDevice = LASTCONTROLLED_KB;
      bMouse3PressedDown   = true;
#ifndef NO_FENG
      if (bUseDynamicFEngSwitching) bConsoleFEng = false;
#endif
      return 0;
    case WM_RBUTTONUP:
      bMouse3PressedDown = false;
      return 0;
    case WM_MBUTTONDOWN:
      LastControlledDevice = LASTCONTROLLED_KB;
      bMouse2PressedDown   = true;
#ifndef NO_FENG
      if (bUseDynamicFEngSwitching) bConsoleFEng = false;
#endif
      return 0;
    case WM_MBUTTONUP:
      bMouse2PressedDown = false;
      return 0;
    case WM_MOUSEWHEEL:
      LastControlledDevice = LASTCONTROLLED_KB;
      MouseWheelValue      = (GET_WHEEL_DELTA_WPARAM(wParam));
#ifndef NO_FENG
      if (bUseDynamicFEngSwitching) bConsoleFEng = false;
#endif
      return 0;
    case WM_SETFOCUS:
      // confine mouse within the game window
      if (bConfineMouse) {
        GetWindowRect(*(HWND*)GAME_HWND_ADDR, &windowRect);
        ClipCursor(&windowRect);
      }
      break;
    case WM_KEYDOWN:
      LastControlledDevice = LASTCONTROLLED_KB;
#ifndef NO_FENG
      if (bUseDynamicFEngSwitching) bConsoleFEng = false;
#endif
      break;
    case WM_MOUSEMOVE:
      if (!bUseWin32Cursor) bLastUsedVirtualMouse = false;
      return 0;
    default:
      break;
  }

  return GameWndProc(hWnd, msg, wParam, lParam);
}

ATOM WINAPI RegisterClassHook(WNDCLASSEXA* wcex) {
  (*wcex).hCursor = NULL;  // this is necessary to be able to hide the mouse quickly enough
  return RegisterClassExA(wcex);
}

void DummyFunc() { return; }

void __stdcall DummyFuncStd(BOOL dummy) { return; }

#endif  // GAME_UC

#pragma runtime_checks("", restore)

#ifdef GAME_UG2
// ---------------------------------------------------------------------------------------------------------------------
// Underground 2 front end button hints
//
// The PC front end shows bare action texts ("Continue", "Back", ...). Like the Widescreen Fix's ImproveGamepadSupport this
// swaps them for "(A) Continue" while a pad is the last used device, but the button comes from the INI binding of the
// FE event the text belongs to, so remapped buttons show up too. The Widescreen Fix option can not be used with this
// plugin: it only knows a pad is in use by reading DirectInput, which the plugin turns off.
// ---------------------------------------------------------------------------------------------------------------------
#include "plugin/PatternScan.h"
#include "plugin/Log.h"
#include "game/ActionID.h"
#include "input/XInputConfig.h"
#include "input/InputDevice.h"
#include <injector/injector.hpp>

bool bShowPadHints      = true;   // "(A) Continue" while the pad is the last used device
bool bShowKeyboardHints = false;  // "[Enter] Continue" while the keyboard is

namespace {

struct MenuHint {
  const char*  text;      // the text the game asks for
  unsigned int joyEvent;  // the FE event that triggers it
};

const MenuHint kMenuHints[] = {
    {"Quit Game", JOY_EVENT_FENG_QUIT},
    {"Continue", JOY_EVENT_FENG_SELECT},
    {"Back", JOY_EVENT_FENG_CANCEL},
    {"Reset Keys To Default", JOY_EVENT_FENG_BUTTON0},
    {"Activate GPS", JOY_EVENT_FENG_BUTTON1},
    {"Deactivate GPS", JOY_EVENT_FENG_BUTTON1},
    {"Install Package", JOY_EVENT_FENG_SELECT},
    {"Install Part", JOY_EVENT_FENG_SELECT},
    {"Install Paint", JOY_EVENT_FENG_SELECT},
    {"Install Decal", JOY_EVENT_FENG_SELECT},
    {"Reset To Default", JOY_EVENT_FENG_BUTTON1},
    {"Delete Tuned Car", JOY_EVENT_FENG_BUTTON1},
    {"Logoff", JOY_EVENT_FENG_CANCEL},
    {"Cancel Changes", JOY_EVENT_FENG_BUTTON1},
    {"Customize", JOY_EVENT_FENG_BUTTON1},
    {"Host LAN Server", JOY_EVENT_FENG_BUTTON1},
    {"Read Message", JOY_EVENT_FENG_SELECT},
    {"Delete", JOY_EVENT_FENG_BUTTON1},
    {"Test N2O Purge", JOY_EVENT_FENG_BUTTON1},
    {"Accept", JOY_EVENT_FENG_SELECT},
    {"Reset to default", JOY_EVENT_FENG_BUTTON1},
    {"Select", JOY_EVENT_FENG_SELECT},
    {"Open/Close Doors", JOY_EVENT_FENG_BUTTON1},
    {"Open/Close Hood", JOY_EVENT_FENG_BUTTON1},
};
constexpr size_t kMenuHintCount = sizeof(kMenuHints) / sizeof(kMenuHints[0]);

// name of a pad binding (button mask or axis id) in the icon set that is in use
const char* PadButtonName(unsigned int bind, bool playStation) {
  switch (bind) {
    case XINPUT_GAMEPAD_DPAD_UP:
      return "D-pad Up";
    case XINPUT_GAMEPAD_DPAD_DOWN:
      return "D-pad Down";
    case XINPUT_GAMEPAD_DPAD_LEFT:
      return "D-pad Left";
    case XINPUT_GAMEPAD_DPAD_RIGHT:
      return "D-pad Right";
    case XINPUT_GAMEPAD_START:
      return playStation ? "Options" : "Start";
    case XINPUT_GAMEPAD_BACK:
      return playStation ? "Share" : "Back";
    case XINPUT_GAMEPAD_LEFT_THUMB:
      return playStation ? "L3" : "LS";
    case XINPUT_GAMEPAD_RIGHT_THUMB:
      return playStation ? "R3" : "RS";
    case XINPUT_GAMEPAD_LEFT_SHOULDER:
      return playStation ? "L1" : "LB";
    case XINPUT_GAMEPAD_RIGHT_SHOULDER:
      return playStation ? "R1" : "RB";
    case XINPUT_GAMEPAD_A:
      return playStation ? "Cross" : "A";
    case XINPUT_GAMEPAD_B:
      return playStation ? "Circle" : "B";
    case XINPUT_GAMEPAD_X:
      return playStation ? "Square" : "X";
    case XINPUT_GAMEPAD_Y:
      return playStation ? "Triangle" : "Y";
    case XINPUT_GAMEPAD_LT_CONFIGDEF:
      return playStation ? "L2" : "LT";
    case XINPUT_GAMEPAD_RT_CONFIGDEF:
      return playStation ? "R2" : "RT";
    case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
    case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
      return "Left stick";
    case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
    case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
      return "Right stick";
    case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
      return "LS Up";
    case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
      return "LS Down";
    case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
      return "LS Left";
    case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
      return "LS Right";
    case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
      return "RS Up";
    case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
      return "RS Down";
    case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
      return "RS Left";
    case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
      return "RS Right";
  }
  return NULL;
}

// name of a keyboard binding: the common keys by hand, the rest as Windows names them
void KeyName(unsigned int vk, char* out, size_t size) {
  switch (vk) {
    case VK_RETURN:
      strncpy_s(out, size, "Enter", _TRUNCATE);
      return;
    case VK_ESCAPE:
      strncpy_s(out, size, "Esc", _TRUNCATE);
      return;
    case VK_SPACE:
      strncpy_s(out, size, "Space", _TRUNCATE);
      return;
    case VK_BACK:
      strncpy_s(out, size, "Backspace", _TRUNCATE);
      return;
    case VK_TAB:
      strncpy_s(out, size, "Tab", _TRUNCATE);
      return;
  }
  if ((vk >= '0' && vk <= '9') || (vk >= 'A' && vk <= 'Z')) {
    out[0] = (char)vk;
    out[1] = 0;
    return;
  }
  LONG scan = (LONG)MapVirtualKeyA(vk, MAPVK_VK_TO_VSC) << 16;
  if (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN || vk == VK_PRIOR || vk == VK_NEXT || vk == VK_HOME || vk == VK_END || vk == VK_INSERT || vk == VK_DELETE)
    scan |= 1 << 24;  // extended key
  if (!scan || !GetKeyNameTextA(scan, out, (int)size)) _snprintf_s(out, size, _TRUNCATE, "0x%02X", vk);
}

// the game's menu text -> the same text with the button hint in front of it. Returns the same pointer when nothing applies
const char* __cdecl TranslateMenuText(const char* text) {
  if (!text) return text;
  static char buffers[kMenuHintCount][96];

  for (size_t i = 0; i < kMenuHintCount; i++) {
    if (strcmp(text, kMenuHints[i].text) != 0) continue;

    char hint[40] = {0};
    if (LastControlledDevice == LASTCONTROLLED_CONTROLLER) {
      if (!bShowPadHints) return text;
      const bool  playStation = (ControllerIconMode == CONTROLLERICON_PS4 || ControllerIconMode == CONTROLLERICON_PS3);
      const char* name        = PadButtonName(UndergroundPadBinding(kMenuHints[i].joyEvent), playStation);
      if (!name) return text;
      _snprintf_s(hint, sizeof(hint), _TRUNCATE, "(%s)", name);
    } else {
      if (!bShowKeyboardHints) return text;
      const unsigned int vk = UndergroundKeyBinding(kMenuHints[i].joyEvent);
      if (!vk) return text;
      char key[32];
      KeyName(vk, key, sizeof(key));
      _snprintf_s(hint, sizeof(hint), _TRUNCATE, "[%s]", key);
    }
    _snprintf_s(buffers[i], sizeof(buffers[i]), _TRUNCATE, "%s %s", hint, text);
    return buffers[i];
  }
  return text;
}

uintptr_t MenuTextGlobal = 0;  // the game global the replaced instruction reads
uintptr_t MenuTextReturn = 0;

// runs in place of "mov ecx, [global]" in front of the game's menu text print, eax = the text
__declspec(naked) void MenuTextCave() {
  __asm {
    push edx
    push eax
    call TranslateMenuText
    add esp, 4
    pop edx
    mov ecx, MenuTextGlobal
    mov ecx, [ecx]
    jmp MenuTextReturn
  }
}

}  // namespace

void InstallUndergroundButtonHints() {
  // same place the Widescreen Fix patches for ImproveGamepadSupport (v1.2: 0x0051257D)
  uint8_t* site = FindPattern("8B 0D ? ? ? ? 85 C9 50 74 12 8B 44");
  if (!site) {
    Log("Button hints: menu text hook not found (another plugin, e.g. the Widescreen Fix with ImproveGamepadSupport, may have patched it already)");
    return;
  }
  MenuTextGlobal = *(uintptr_t*)(site + 2);
  MenuTextReturn = (uintptr_t)site + 6;
  injector::MakeJMP((uintptr_t)site, MenuTextCave, true);
  injector::MakeNOP((uintptr_t)site + 5, 1, true);
  Log("Button hints: hooked the menu text at %p", site);
}
#endif  // GAME_UG2
