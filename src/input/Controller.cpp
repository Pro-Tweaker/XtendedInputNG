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
#include "input/Controller.h"
#include "input/GamepadBackend.h"
#include "input/AdaptiveTriggers.h"
#include "input/Rumble.h"
#include "input/XInputConfig.h"
#include "plugin/Plugin.h"
#include "input/DebugCamera.h"
#include "input/InputDevice.h"
#include "fe/FEng.h"

bool  bRadialDeadzone                  = true;
SHORT INPUT_DEADZONE_LS            = PercentToAxis(0.24f);
SHORT INPUT_DEADZONE_RS            = PercentToAxis(0.24f);
SHORT INPUT_DEADZONE_LS_P2         = PercentToAxis(0.24f);
SHORT INPUT_DEADZONE_RS_P2         = PercentToAxis(0.24f);
SHORT SHIFT_ANALOG_THRESHOLD       = PercentToAxis(0.75f);  // 75% for shifting
SHORT FEUPDOWN_ANALOG_THRESHOLD    = PercentToAxis(0.50f);  // 50% for analog sticks digital activation
BYTE  TRIGGER_ACTIVATION_THRESHOLD = PercentToTrigger(0.12f);  // 12% for analog triggers digital activation

// for triggering the over-zelaous inputs once in a tick...
WORD bQuitButtonOldState = 0;
CONTROLLER_STATE g_Controllers[MAX_CONTROLLERS];
bool bXInputOmniMode = false;
bool bPassConnStatus = true;

// KB Input declarations
BYTE         VKeyStates[2][256];
BYTE         AsyncKeyCache[256];  // 0 = not read this poll, 1 = up, 2 = down
unsigned int KeyboardReadingMode = 0;  // 0 = unbuffered asynchronous, 1 = buffered synchronous
bool         bMouseLook          = true;
bool bEnteredWorldMapOnce = false;
ULONGLONG           g_ControllerNextPoll[MAX_CONTROLLERS];

static bool PollController(int index) {
  ULONGLONG now = GetTickCount64();
  if (!g_Controllers[index].bConnected && now < g_ControllerNextPoll[index]) return false;

  bool bOk = GetGamepadBackend()->Poll(index, g_Controllers[index].state);
  if (!bOk) g_ControllerNextPoll[index] = now + CONTROLLER_REPOLL_INTERVAL_MS;
  return bOk;
}

// circular deadzone: the stick vector is cut as a whole and the remaining range is stretched back to full
static void ApplyRadialDeadzone(SHORT& x, SHORT& y, SHORT deadzone) {
  const float fx = (float)x, fy = (float)y;
  const float mag = std::sqrt(fx * fx + fy * fy);
  if (mag < (float)deadzone) {
    x = 0;
    y = 0;
    return;
  }
  const float limit = (float)SHORT_MAX;
  if (limit <= (float)deadzone) return;
  const float scaled = (std::min(mag, limit) - (float)deadzone) / (limit - (float)deadzone) * limit;
  const float k      = scaled / mag;
  x                  = (SHORT)clampf(fx * k, -limit, limit);
  y                  = (SHORT)clampf(fy * k, -limit, limit);
}

static void ApplyDeadzone(XINPUT_GAMEPAD& pad, SHORT deadzoneL, SHORT deadzoneR) {
  if (bRadialDeadzone) {
    ApplyRadialDeadzone(pad.sThumbLX, pad.sThumbLY, deadzoneL);
    ApplyRadialDeadzone(pad.sThumbRX, pad.sThumbRY, deadzoneR);
    return;
  }
  // widen to int first, -32768 can't be negated as a SHORT
  if (std::abs((int)pad.sThumbLX) < deadzoneL) pad.sThumbLX = 0;
  if (std::abs((int)pad.sThumbLY) < deadzoneL) pad.sThumbLY = 0;
  if (std::abs((int)pad.sThumbRX) < deadzoneR) pad.sThumbRX = 0;
  if (std::abs((int)pad.sThumbRY) < deadzoneR) pad.sThumbRY = 0;
}

static bool PadHasInput(const XINPUT_GAMEPAD& pad) {
  return pad.wButtons || pad.sThumbLX || pad.sThumbLY || pad.sThumbRX || pad.sThumbRY || pad.bRightTrigger || pad.bLeftTrigger;
}

static void MarkControllerActive(int slot) {
  LastControlledDevice = LASTCONTROLLED_CONTROLLER;
#if !defined(NO_FENG) || defined(GAME_UG2)
  if (bAutoControllerIcons) {
    int mode = GetGamepadBackend()->IconModeForSlot(slot);
    if (mode >= 0) ControllerIconMode = (unsigned int)mode;
  }
#endif
#ifndef NO_FENG
  if (bUseDynamicFEngSwitching) bConsoleFEng = true;
#endif
}

// keeps the axis with the biggest deflection
static SHORT MergeAxis(SHORT a, SHORT b) {
  return std::abs((int)a) >= std::abs((int)b) ? a : b;
}

HRESULT UpdateControllerState() {
  GetGamepadBackend()->BeginFrame();

  if (bXInputOmniMode) {
    XINPUT_STATE omni_state  = {0};
    bool         bConnStatus = false;

    for (int i = 0; i < MAX_CONTROLLERS; i++) {
      if (!PollController(i)) {
        g_Controllers[i].bConnected = false;
        continue;
      }
      g_Controllers[i].bConnected = true;
      bConnStatus                 = true;

      auto& state = g_Controllers[i].state;
      ApplyDeadzone(state.Gamepad, INPUT_DEADZONE_LS, INPUT_DEADZONE_RS);

      if (PadHasInput(state.Gamepad)) {
        MarkControllerActive(i);

        // combine states into one
        omni_state.Gamepad.wButtons |= state.Gamepad.wButtons;
        omni_state.Gamepad.sThumbLX = MergeAxis(omni_state.Gamepad.sThumbLX, state.Gamepad.sThumbLX);
        omni_state.Gamepad.sThumbLY = MergeAxis(omni_state.Gamepad.sThumbLY, state.Gamepad.sThumbLY);
        omni_state.Gamepad.sThumbRX = MergeAxis(omni_state.Gamepad.sThumbRX, state.Gamepad.sThumbRX);
        omni_state.Gamepad.sThumbRY = MergeAxis(omni_state.Gamepad.sThumbRY, state.Gamepad.sThumbRY);
        omni_state.Gamepad.bRightTrigger = std::max(omni_state.Gamepad.bRightTrigger, state.Gamepad.bRightTrigger);
        omni_state.Gamepad.bLeftTrigger  = std::max(omni_state.Gamepad.bLeftTrigger, state.Gamepad.bLeftTrigger);
      }
    }
    // both player slots mirror the merged state (not a bug)
    g_Controllers[0].bConnected = bConnStatus;
    g_Controllers[1].bConnected = bConnStatus;
    memcpy(&(g_Controllers[0].state), &omni_state, sizeof(XINPUT_STATE));
    memcpy(&(g_Controllers[1].state), &omni_state, sizeof(XINPUT_STATE));
  }

  else {
    if (PollController(0)) {
      g_Controllers[0].bConnected = true;
      ApplyDeadzone(g_Controllers[0].state.Gamepad, INPUT_DEADZONE_LS, INPUT_DEADZONE_RS);
      if (PadHasInput(g_Controllers[0].state.Gamepad)) MarkControllerActive(0);
    } else
      g_Controllers[0].bConnected = false;

    if (PollController(1)) {
      g_Controllers[1].bConnected = true;
      ApplyDeadzone(g_Controllers[1].state.Gamepad, INPUT_DEADZONE_LS_P2, INPUT_DEADZONE_RS_P2);
    } else {
      g_Controllers[1].bConnected = false;
    }
  }

  UpdateAdaptiveTriggers();
  UpdateRumble();
  return S_OK;
}

// old function -- repurposed to send keyboard-exclusive commands to the game
void ReadXInput_Extra() {
#ifndef NO_QUIT_BUTTON
  if (g_Controllers[0].bConnected && bGlobalDoPolling) {
    WORD wButtons = g_Controllers[0].state.Gamepad.wButtons;

#ifdef GAME_UG
    // Underground 1 takes keys for these front end actions through a game variable
    if (*(int*)GAMEFLOWMANAGER_STATUS_ADDR == 3) {
      if (wButtons & XINPUT_GAMEPAD_Y) *(char*)FE_WINDOWS_KEY_CODE_ADDR = 'D';  // delete profile
      if (g_Controllers[0].state.Gamepad.bLeftTrigger > TRIGGER_ACTIVATION_THRESHOLD) *(char*)FE_WINDOWS_KEY_CODE_ADDR = 'T';  // tutorial
    }
#endif

    if ((wButtons & XINPUT_GAMEPAD_BACK) != bQuitButtonOldState) {
      if ((wButtons & XINPUT_GAMEPAD_BACK))  // trigger once only on button down state
      {
        FESendKeystroke('Q');
      }
      bQuitButtonOldState = (wButtons & XINPUT_GAMEPAD_BACK);
    }

#ifdef GAME_MW
    // simulate mouse cursor for map movement
    if (*(int*)GAMEFLOWMANAGER_STATUS_ADDR == 6) {
      if (g_Controllers[0].state.Gamepad.sThumbRX || g_Controllers[0].state.Gamepad.sThumbRY) bLastUsedVirtualMouse = true;

      if (bLastUsedVirtualMouse && LastControlledDevice == LASTCONTROLLED_CONTROLLER && cFEng_FindPackage("WorldMapMain.fng")) {
        if (bUseWin32Cursor) {
          if (!bEnteredWorldMapOnce && !bConfineMouse)  // force confine a mouse if not already, we don't want the mouse to stray away from the game if we're
                                                        // using a controller
          {
            RECT windowRect;
            GetWindowRect(*(HWND*)GAME_HWND_ADDR, &windowRect);
            ClipCursor(&windowRect);
          }

          POINT MousePos;
          GetCursorPos(&MousePos);
          SetCursorPos(MousePos.x + ((g_Controllers[0].state.Gamepad.sThumbRX) / VIRTUAL_MOUSE_STICK_DIVISOR), MousePos.y + (-g_Controllers[0].state.Gamepad.sThumbRY / VIRTUAL_MOUSE_STICK_DIVISOR));
        } else {
          *(int*)FEMOUSECURSOR_X_ADDR += (g_Controllers[0].state.Gamepad.sThumbRX / VIRTUAL_MOUSE_STICK_DIVISOR);
          *(int*)FEMOUSECURSOR_Y_ADDR += (-g_Controllers[0].state.Gamepad.sThumbRY / VIRTUAL_MOUSE_STICK_DIVISOR);
        }

        bMousePressedDown    = g_Controllers[0].state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
        bEnteredWorldMapOnce = true;
      } else {
        if (bEnteredWorldMapOnce && !bConfineMouse && bUseWin32Cursor) ClipCursor(NULL);
        bEnteredWorldMapOnce  = false;
        bLastUsedVirtualMouse = false;
      }
    } else {
      if (bEnteredWorldMapOnce && !bConfineMouse && bUseWin32Cursor) ClipCursor(NULL);
      bEnteredWorldMapOnce  = false;
      bLastUsedVirtualMouse = false;
    }
#endif
  }
#endif  // GAME_UC
}

// unbuffered mode reads each bound key at most once per poll
bool IsAsyncKeyDown(BYTE vkey) {
  if (!AsyncKeyCache[vkey]) AsyncKeyCache[vkey] = (GetAsyncKeyState(vkey) >> 15) ? 2 : 1;
  return AsyncKeyCache[vkey] == 2;
}

void __stdcall ReadControllerData() {
  memset(AsyncKeyCache, 0, sizeof(AsyncKeyCache));
  UpdateControllerState();
  ReadXInput_Extra();
  if (KeyboardReadingMode == KB_READINGMODE_BUFFERED) GetKeyboardState(VKeyStates[0]);
#ifdef GAME_UNDERGROUND
  UpdateUndergroundDevices();
#endif

  // read mouse info for Debug World Camera
  if (bInDebugWorldCamera && bGlobalDoPolling) {
    RECT  windowRect;
    POINT MousePos;
    GetCursorPos(&MousePos);
    GetWindowRect(*(HWND*)GAME_HWND_ADDR, &windowRect);
    int CenterX = ((windowRect.right - windowRect.left) / 2) + windowRect.left;
    int CenterY = ((windowRect.bottom - windowRect.top) / 2) + windowRect.top;

#ifndef GAME_WORLD
    // add mouse speed to the original speed
    MouseLook_XSpeed = -(int)((float)((MousePos.x - CenterX) * 1000) * MouseLookSensitivity);
    MouseLook_YSpeed = -(int)((float)((MousePos.y - CenterY) * 1000) * MouseLookSensitivity);
#else
    MouseLook_XSpeed = -(int)((float)((MousePos.x - CenterX) * 100) * MouseLookSensitivity);
    MouseLook_YSpeed = -(int)((float)((MousePos.y - CenterY) * 100) * MouseLookSensitivity);
#endif
    SetCursorPos(CenterX, CenterY);
  }
}

float GetAnalogStickValue(int index, WORD bind) {
  float result = 0.0f;
  int   rdi    = index;
  switch (bind) {
    case XINPUT_GAMEPAD_LT_CONFIGDEF:
      result = g_Controllers[rdi].state.Gamepad.bLeftTrigger / 255.0f;
      break;
    case XINPUT_GAMEPAD_RT_CONFIGDEF:
      result = g_Controllers[rdi].state.Gamepad.bRightTrigger / 255.0f;
      break;
    case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
      result = g_Controllers[rdi].state.Gamepad.sThumbLX / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
      result = g_Controllers[rdi].state.Gamepad.sThumbRX / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
      result = g_Controllers[rdi].state.Gamepad.sThumbLY / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
      result = g_Controllers[rdi].state.Gamepad.sThumbRY / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbLY > 0) result = g_Controllers[rdi].state.Gamepad.sThumbLY / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbLY < 0) result = -g_Controllers[rdi].state.Gamepad.sThumbLY / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbLX < 0) result = -g_Controllers[rdi].state.Gamepad.sThumbLX / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbLX > 0) result = g_Controllers[rdi].state.Gamepad.sThumbLX / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbRY > 0) result = g_Controllers[rdi].state.Gamepad.sThumbRY / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbRY < 0) result = -g_Controllers[rdi].state.Gamepad.sThumbRY / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbRX < 0) result = -g_Controllers[rdi].state.Gamepad.sThumbRX / (float)(0x7FFF);
      break;
    case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
      if (g_Controllers[rdi].state.Gamepad.sThumbRX > 0) result = g_Controllers[rdi].state.Gamepad.sThumbRX / (float)(0x7FFF);
      break;
    default:
      break;
  }
  return result;
}

uint16_t GetAnalogActivity() {
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_LT_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_LT_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_RT_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_RT_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_LS_UP_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_LS_UP_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_RS_UP_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_RS_UP_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF;
  if (GetAnalogStickValue(0, XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF) >= 1.0f) return XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF;
  return 0;
}

bool bGetAnalogDigitalActivationState(int index, WORD bind, int actid) {
  WORD threshold = FEUPDOWN_ANALOG_THRESHOLD;
#ifdef GAME_UNDERGROUND
  // Underground passes the JOY_EVENT instead of an ActionID
  if (actid == JOY_EVENT_SHIFTUP || actid == JOY_EVENT_SHIFTDOWN || actid == JOY_EVENT_SHIFTUP_ALTERNATE || actid == JOY_EVENT_SHIFTDOWN_ALTERNATE) threshold = SHIFT_ANALOG_THRESHOLD;
#else
  if (actid == GAMEACTION_SHIFTUP || actid == GAMEACTION_SHIFTDOWN) threshold = SHIFT_ANALOG_THRESHOLD;
#endif

  bool bCurrentXInputKeyState = false;
  int  rdi                    = index;
  switch (bind) {
    case XINPUT_GAMEPAD_LT_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.bLeftTrigger > TRIGGER_ACTIVATION_THRESHOLD);
      break;
    case XINPUT_GAMEPAD_RT_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.bRightTrigger > TRIGGER_ACTIVATION_THRESHOLD);
      break;
    case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbLY > threshold);
      break;
    case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbLY < -threshold);
      break;
    case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbLX < -threshold);
      break;
    case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbLX > threshold);
      break;
    case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbRY > threshold);
      break;
    case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbRY < -threshold);
      break;
    case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbRX < -threshold);
      break;
    case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
      bCurrentXInputKeyState = (g_Controllers[rdi].state.Gamepad.sThumbRX > threshold);
      break;
  }

  return bCurrentXInputKeyState;
}
