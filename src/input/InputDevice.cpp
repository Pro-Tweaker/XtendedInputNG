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
#include "input/InputDevice.h"
#include "plugin/Plugin.h"
#include "game/ActionID.h"
#include "input/XInputConfig.h"
#include "input/Controller.h"
#include "input/Mapping.h"
#include "fe/FEng.h"

#ifndef GAME_UNDERGROUND

#pragma runtime_checks("", off)

void*(__thiscall* UTL_Com_Object_IList_Constructor)(void* thethis, unsigned int unk) = (void*(__thiscall*)(void*, unsigned int))UTL_ILIST_CONSTRUCTOR_ADDR;

// global var for the action ID values
float PrevValues[2][MAX_ACTIONID];
float CurrValues[2][MAX_ACTIONID];

#ifndef GAME_WORLD
bool bIsHudVisible() {
  if (*(int*)GAMEFLOWMANAGER_STATUS_ADDR == 6) {
    int FirstLocalPlayer = **(int**)PLAYER_ILISTABLE_ADDR;
    if (FirstLocalPlayer) {
      int LocalPlayerVtable                           = *(int*)(FirstLocalPlayer);
      int(__thiscall * LocalPlayer_GetHUD)(void* dis) = (int(__thiscall*)(void*)) * (int*)(LocalPlayerVtable + PLAYER_GETHUD_VTABLE_OFFSET);
      int LocalPlayerHUD                              = LocalPlayer_GetHUD((void*)FirstLocalPlayer);
      if (LocalPlayerHUD) {
        int LocalPlayerHUDVtable                          = *(int*)(LocalPlayerHUD);
        int(__thiscall * FEngHud_IsHudVisible)(void* dis) = (int(__thiscall*)(void*)) * (int*)(LocalPlayerHUDVtable + FENGHUD_ISVISIBLE_VTABLE_OFFSET);

        if (FEngHud_IsHudVisible((void*)LocalPlayerHUD))  // is HUD being drawn at all
          return true;
      }
    }
  }
  return false;
}

#else
bool bIsHudVisible() { return false; }

// world, for whatever reason, uses direct pointers to the class instead of accessing the device scalar array normally
// copied this code snippet from Carbon 0x00696166
unsigned int loc_75E375 = 0x75E375 + MainBase;
unsigned int loc_75E363 = 0x75E363 + MainBase;

void __declspec(naked) UnscrewWorldDeviceScalars() {
  _asm
  {
        mov ecx, [ebx+0x18]
        shl esi, 4
        add esi, ecx
        test esi, esi
        jnz unscrew_jnz
        jmp loc_75E363
        unscrew_jnz:
        jmp loc_75E375
  }
}

unsigned int loc_75E237 = 0x75E237 + MainBase;
unsigned int loc_75E225 = 0x75E225 + MainBase;

void __declspec(naked) UnscrewWorldDeviceScalars2() {
  _asm
  {
        mov ecx, [ebx + 0x18]
        shl edi, 4
        add edi, ecx
        mov [ebp-8], edi
        test edi, edi
        jnz unscrew2_jnz
        jmp loc_75E225
        unscrew2_jnz :
        jmp loc_75E237
  }
}

unsigned int loc_75E25F = 0x75E25F + MainBase;

void __declspec(naked) UnscrewWorldDeviceScalars3() {
  _asm
  {
        mov edx, [ebx + 0x18]
        shl edi, 4
        add edi, edx
        jmp loc_75E25F
  }
}

#endif
bool bCheckSecondBind() { return FE_Secondary_Up || FE_Secondary_Down || FE_Secondary_Left || FE_Secondary_Right; }

InputDevice::InputDevice(int DeviceIndex) {
    memset(&Padding1, 0, sizeof(long) * 5);
    UTL_Com_Object_IList_Constructor(&Padding1, 4);
    fDeviceIndex     = DeviceIndex;
    fControllerCurve = 1.0f;
    fPrevValues      = PrevValues[DeviceIndex];
    fCurrentValues   = CurrValues[DeviceIndex];
    fDeviceScalar    = new (std::nothrow) DeviceScalar[MAX_ACTIONID];
#ifdef GAME_PROSTREET
    bRemappingMode = false;
    RemapType      = 0;
    RemapIndex     = 0;
    RemapIsPrimary = 0;
#endif
#ifndef NO_PROFILE_SETTINGS
    InitProfileSettings();
#endif
  }

void* InputDevice::dtor(bool something) {
    delete[] fDeviceScalar;
    fDeviceScalar = NULL;
    delete (this);
    return this;
  }

bool InputDevice::IsConnected() {
    if ((LastControlledDevice == LASTCONTROLLED_CONTROLLER) && bPassConnStatus) return g_Controllers[fDeviceIndex].bConnected;
    return true;
  }

void InputDevice::Initialize() {
    if (!fDeviceScalar) return;  // allocation failed
    // printf("Called InputDevice::Initialize\n");
    //*(bool*)FEMOUSECURSOR_ISHIDDEN_ADDR = false;

    for (unsigned int i = 0; i < MAX_ACTIONID; i++) {
      if (bIsActionDigitalButton((ActionID)i))
        fDeviceScalar[i].fType = kDigitalButton;
      else
        fDeviceScalar[i].fType = kJoyAxis;

      fDeviceScalar[i].fName_CRC32   = stringhash32(ActionIDStr[i]);
      fDeviceScalar[i].fPrevValue    = &PrevValues[fDeviceIndex][i];
      fDeviceScalar[i].fCurrentValue = &CurrValues[fDeviceIndex][i];
    }
#if defined(GAME_UC) || defined(GAME_WORLD)
    LoadMapping(NFSProfileName);
#else
    LoadMapping("");
#endif
  }

void InputDevice::PollDevice() {
    if (!bGlobalDoPolling) return;

    if (bBlockThisPoll) {
      bBlockThisPoll = false;
      return;
    }

    ReadControllerData();
    WORD wButtons                    = g_Controllers[fDeviceIndex].state.Gamepad.wButtons;
    WORD SecondBind                  = 0;
    int  rdi                         = fDeviceIndex;
    bool bCurrentXInputKeyState      = false;
    bool bCurrentXInputKeyState2     = false;
    bool bCurrentVKeyState           = false;
    bool bCurrentVKeyState_SECONDARY = false;
    bool bDoPolling                  = true;

    // printf("FEngPkg: %s\n", cFEng_FindPackageWithControl_Name());

#ifdef GAME_MW
    bool bInShowcase = false;
    if (*(int*)GAMEFLOWMANAGER_STATUS_ADDR == 3) {
      if (cFEng_FindPackage(FE_SHOWCASE_FNG_NAME) || cFEng_FindPackage("UI_DebugCarCustomize.fng") || *(bool*)CARGUYSCAMERA_ADDR) bInShowcase = true;
    }
#endif
#ifdef GAME_PROSTREET
    if (bRemappingMode) {
      if (RemapType == 1) {
        if (HandleKeyboardRemap(RemapIndex, RemapIsPrimary)) {
          g_bRemapClear           = false;
          g_bRemapClear_IsPrimary = std::numeric_limits<std::uint32_t>::max();
          g_iRemapClear_BtnIndex  = std::numeric_limits<std::uint32_t>::max();
          bRemappingMode          = false;
        }
      }

      if (RemapType == 3) {
        if (HandleGamepadRemap(RemapIndex, RemapIsPrimary, fDeviceIndex)) {
          g_bRemapClear           = false;
          g_bRemapClear_IsPrimary = std::numeric_limits<std::uint32_t>::max();
          g_iRemapClear_BtnIndex  = std::numeric_limits<std::uint32_t>::max();
          bRemappingMode          = false;
        }
      }
    }
#endif
    for (unsigned int j = 0; j < MAX_ACTIONID; j++) {
      ActionID i = (ActionID)j;

#ifdef GAME_UC
      if (bIsActionFrontEnd((ActionID)i) && bIsHudVisible()) bDoPolling = false;
#else
      if (bIsActionFrontEnd((ActionID)i) && bIsHudVisible() && (i != FRONTENDACTION_START)) bDoPolling = false;
#endif

#ifdef GAME_WORLD
      // I *truly* do not understand this game. So, the inputs will only start working if a button was pressed BEFORE entering the game.
      // So I'll set an action to a high state while in FE. FE isn't even controlled by the controller, so it should be safe.
      if (*(int*)GAMEFLOWMANAGER_STATUS_ADDR != 6) {
        CurrValues[fDeviceIndex][i] = 1.0;
        bDoPolling                  = false;
      }
#endif

      if (bDoPolling) {
        float fresult = 0;
        rdi           = fDeviceIndex;
        if (VKeyBindings_PRIMARY[i]) {
          if (KeyboardReadingMode == KB_READINGMODE_BUFFERED)
            bCurrentVKeyState = VKeyStates[0][VKeyBindings_PRIMARY[i]] >> 7;
          else
            bCurrentVKeyState = IsAsyncKeyDown(VKeyBindings_PRIMARY[i]);
        } else
          bCurrentVKeyState = false;

        if (VKeyBindings_SECONDARY[i]) {
          if (KeyboardReadingMode == KB_READINGMODE_BUFFERED)
            bCurrentVKeyState_SECONDARY = VKeyStates[0][VKeyBindings_SECONDARY[i]] >> 7;
          else
            bCurrentVKeyState_SECONDARY = IsAsyncKeyDown(VKeyBindings_SECONDARY[i]);
        } else
          bCurrentVKeyState_SECONDARY = false;

        bCurrentVKeyState |= bCurrentVKeyState_SECONDARY;

        if (bIsActionDebug((ActionID)i)) {
          rdi = 1;  // debug (camera) actions are always read from the second port, but the PC version omits it entirely so this is a workaround
        }

        wButtons = g_Controllers[rdi].state.Gamepad.wButtons;

        if (bIsActionAnalog((ActionID)i)) {
          if (bIsBindingAnalog(XInputBindings_PRIMARY[i])) {
            fresult = GetAnalogStickValue(rdi, XInputBindings_PRIMARY[i]);
          } else {
            bCurrentXInputKeyState = wButtons & XInputBindings_PRIMARY[i];
            // keyboard & digital buttons
            if (bCurrentXInputKeyState) fresult = 1.0f;
          }

          if (bIsBindingAnalog(XInputBindings_SECONDARY[i])) {
            if (fresult == 0.0f) fresult = GetAnalogStickValue(rdi, XInputBindings_SECONDARY[i]);
          } else {
            if (!bCurrentXInputKeyState) {
              bCurrentXInputKeyState = wButtons & XInputBindings_SECONDARY[i];
              // keyboard & digital buttons
              if (bCurrentXInputKeyState) fresult = 1.0f;
            }
          }

          if (bCurrentVKeyState) fresult = 1.0f;
        } else {
          if (bCheckSecondBind() && bIsActionFrontEndNavigation((ActionID)i)) {
            switch (i) {
              case FRONTENDACTION_UP:
                SecondBind = FE_Secondary_Up;
                break;
              case FRONTENDACTION_DOWN:
                SecondBind = FE_Secondary_Down;
                break;
              case FRONTENDACTION_LEFT:
                SecondBind = FE_Secondary_Left;
                break;
              case FRONTENDACTION_RIGHT:
                SecondBind = FE_Secondary_Right;
                break;
              default:
                SecondBind = 0;
                break;
            }

            if (bIsBindingAnalog(SecondBind))  // digital directions with an axis
            {
              bCurrentXInputKeyState2 = bGetAnalogDigitalActivationState(rdi, SecondBind, i);
            }

            else
              bCurrentXInputKeyState2 = wButtons & SecondBind;
          } else {
            SecondBind              = 0;
            bCurrentXInputKeyState2 = false;
          }

          if (bIsBindingAnalog(XInputBindings_PRIMARY[i]) || bIsBindingAnalog(XInputBindings_SECONDARY[i]))  // digital directions with an axis
          {
            bCurrentXInputKeyState = bGetAnalogDigitalActivationState(rdi, XInputBindings_PRIMARY[i], i);
            if (!bCurrentXInputKeyState) bCurrentXInputKeyState = bGetAnalogDigitalActivationState(rdi, XInputBindings_SECONDARY[i], i);
            if (bCurrentXInputKeyState || bCurrentXInputKeyState2) fresult = 1.0f;
          } else {
            bCurrentXInputKeyState = wButtons & XInputBindings_PRIMARY[i];
            if (!bCurrentXInputKeyState) bCurrentXInputKeyState = wButtons & XInputBindings_SECONDARY[i];
            // determine final digital status
            if (bCurrentXInputKeyState || bCurrentXInputKeyState2) fresult = 1.0f;
          }
          if (bCurrentVKeyState) fresult = 1.0f;
        }

        // write value to the main array
        CurrValues[fDeviceIndex][i] = fresult;

        // update prev values
        if (CurrValues[fDeviceIndex][i] != PrevValues[fDeviceIndex][i]) {
          // do one-time triggers here...
#ifdef GAME_MW
          if (bIsActionRelativePosition((ActionID)i) && bInShowcase) {
            if (CurrValues[fDeviceIndex][i] > 0)
              CurrValues[fDeviceIndex][i] = CurrValues[fDeviceIndex][i] - PrevValues[fDeviceIndex][i];
            else
              CurrValues[fDeviceIndex][i] = 0;
          }
#endif
          PrevValues[fDeviceIndex][i] = CurrValues[fDeviceIndex][i];
        } else if (bIsActionDigitalButton((ActionID)i)) {
          if (CurrValues[fDeviceIndex][i] > 0) CurrValues[fDeviceIndex][i] = 0;
        }
      }
      bDoPolling = true;
    }
  }

#ifdef GAME_PROSTREET
void InputDevice::GoRemapButton(uint32_t type, uint32_t button, uint32_t isPrimary, uint32_t unk) {
    bRemappingMode = true;
    RemapType      = type;
    RemapIndex     = button;
    RemapIsPrimary = isPrimary;
  }
#endif

InputDevice* InputDeviceFactory(int DeviceIndex) {
  InputDevice* out = new InputDevice(DeviceIndex);

  return out;
}

#ifdef GAME_PROSTREET
void __stdcall GoRemapButtonHook(uint32_t type, uint32_t button_index, uint32_t isPrimary, uint32_t unk) {
  InputDevice* that;
  _asm mov     that, ecx;
  return that->GoRemapButton(type, button_index, isPrimary, unk);
}

void __stdcall GoClearButtonHook(uint32_t type, uint32_t button_index, uint32_t isPrimary, uint32_t unk) {
  InputDevice* that;
  _asm mov     that, ecx;
  g_bRemapClear           = true;
  g_bRemapClear_IsPrimary = isPrimary;
  g_iRemapClear_BtnIndex  = button_index;
  return that->GoRemapButton(type, button_index, isPrimary, unk);
}

bool __stdcall GetRemappingModeHook() {
  InputDevice* that;
  _asm mov     that, ecx;

  return that->bRemappingMode;
}

void GetMappingStringHook(char* str, uint32_t type, uint32_t button_index, uint32_t isPrimary, uint32_t unk) {
  if (g_bRemapClear && button_index == g_iRemapClear_BtnIndex && isPrimary == g_bRemapClear_IsPrimary) {
    strcpy(str, "---");
    return;
  }
  if (type == 1) {
    size_t vkey = isPrimary ? VKeyBindings_PRIMARY[FindPCRemapActionID(button_index)] : VKeyBindings_SECONDARY[FindPCRemapActionID(button_index)];
    if (vkey >= ControlsTextsPC_Count) vkey = 0;  // unbound string
    strcpy(str, ControlsTextsPC[vkey]);
    return;
  }

  if (type == 3) {
    strcpy(str, ConvertBitmaskToControlString(ControllerIconMode, isPrimary ? XInputBindings_PRIMARY[FindPCRemapActionID(button_index)]
                                                                            : XInputBindings_SECONDARY[FindPCRemapActionID(button_index)]));
    return;
  }

  strcpy(str, "---");
}

uint32_t GetNumExternalDevices() { return 1; }

uint32_t GetNumExternalDevicesZero() { return 0; }

const char* __stdcall GetXtendedInputNGDeviceName(uint32_t type, uint32_t index) {
  if (type == 1) return "Keyboard";
  if (type == 3) return "XInput Controller";

  return "Unknown Input Device";
}

#endif

#pragma runtime_checks("", restore)

#endif  // !GAME_UNDERGROUND

#ifdef GAME_UNDERGROUND
// ---------------------------------------------------------------------------------------------------------------------
// Underground 1 & 2
//
// These games never build InputDevice/InputMapping objects. Each frame they call one scanner function per JOY_EVENT
// (looked up through a ScannerConfig table) and the scanner returns 0 for "nothing changed" or JoyEvent + (value << 8).
// The Win32 side (pad polling, deadzones, keyboard state) is the shared code in Controller.cpp.
// ---------------------------------------------------------------------------------------------------------------------
#include <mINI/src/mini/ini.h>
#include <injector/injector.hpp>
#include "input/VKHash.h"

// layout is the game's own (the engine reads ScannerFunctionPointer and JoyEvent from it); the last field is ours
struct ScannerConfig {
  unsigned int JoyConfigType          = 0xFF;  // 0xFF default
  unsigned int unk1                   = 0;     // not read
  unsigned int JoyEvent               = 0;
  unsigned int ScannerFunctionPointer = 0;
  uint16_t     BitmaskStuff           = 0;  // XInput button mask, or one of the *_CONFIGDEF axis ids
  uint16_t     BitmaskStuff2          = 0;
  unsigned int unk5                   = 0;
  unsigned int param                  = 0;
  unsigned int keycode                = 0;  // unknown to the game, we keep the keyboard binding here
};

static ScannerConfig ScannerConfigs[MAX_JOY_EVENT];

// last reported state per player and event
static WORD EventStates[2][MAX_JOY_EVENT];
static bool EventStatesKB[2][MAX_JOY_EVENT];

// steering axis, buttons and keys
static WORD   SteerLeftButton  = 0;  // digital steering defaults to nothing, the analog axis does the work
static WORD   SteerRightButton = 0;
static WORD   SteerLeftAxis    = XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF;
static WORD   SteerRightAxis   = XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF;
static USHORT SteerLeftVKey    = VK_LEFT;
static USHORT SteerRightVKey   = VK_RIGHT;

// the game passes the pad's address, player 2 is the second one
static int PadIndex(void* Joystick) { return (int)Joystick == JOY2_ADDR ? 1 : 0; }

// throttle/brake are time sensitive: their digital scanners report on every call, not only on change
static bool IsAlwaysReportedEvent(unsigned int ev) {
  return ev == JOY_EVENT_THROTTLE || ev == JOY_EVENT_BRAKE || ev == JOY_EVENT_THROTTLE_ANALOG || ev == JOY_EVENT_THROTTLE_ANALOG_ALTERNATE || ev == JOY_EVENT_BRAKE_ANALOG ||
         ev == JOY_EVENT_BRAKE_ANALOG_ALTERNATE;
}

// press returns JoyEvent + (param << 8), release returns the bare JoyEvent, no change returns 0
static int DigitalChange(bool down, bool& old, const ScannerConfig* cfg, bool alwaysReport) {
  if (!alwaysReport && down == old) return 0;
  old = down;
  return down ? (int)(cfg->JoyEvent + (cfg->param << 8)) : (int)cfg->JoyEvent;
}

// same, but only the press is reported
static int DigitalDownChange(bool down, bool& old, const ScannerConfig* cfg) {
  if (down == old) return 0;
  old = down;
  return down ? (int)cfg->JoyEvent : 0;
}

// the keyboard is read the way KeyboardReadingMode says (0 async, 1 buffered). Raw input (2) is not ported and reads like 0
static bool IsKeyDown(unsigned int keycode) {
  if (!keycode) return false;
  if (KeyboardReadingMode == KB_READINGMODE_BUFFERED) return (VKeyStates[0][keycode] >> 7) != 0;
  return IsAsyncKeyDown((BYTE)keycode);
}

// ---- digital buttons ----------------------------------------------------------------------------------------------

// pad button, with or without release events
static int ScanPadButton(const ScannerConfig* cfg, void* Joystick, bool downOnly) {
  const int  ci   = PadIndex(Joystick);
  const bool down = (g_Controllers[ci].state.Gamepad.wButtons & cfg->BitmaskStuff) != 0;
  bool       old  = EventStates[ci][cfg->JoyEvent] != 0;
  const int  ret  = downOnly ? DigitalDownChange(down, old, cfg) : DigitalChange(down, old, cfg, IsAlwaysReportedEvent(cfg->JoyEvent));
  EventStates[ci][cfg->JoyEvent] = old;
  return ret;
}

static int ScanKey(const ScannerConfig* cfg, bool downOnly) {
  bool& old  = EventStatesKB[0][cfg->JoyEvent];
  bool  down = IsKeyDown(cfg->keycode);
  if (down) LastControlledDevice = LASTCONTROLLED_KB;  // what the button hints follow
  return downOnly ? DigitalDownChange(down, old, cfg) : DigitalChange(down, old, cfg, IsAlwaysReportedEvent(cfg->JoyEvent));
}

static int Scanner_DigitalUpOrDown(void*, unsigned int*, unsigned int, ScannerConfig* cfg, void* Joystick) {
  return ScanPadButton(cfg, Joystick, false) | ScanKey(cfg, false);
}

// same as DigitalUpOrDown except no release events
static int Scanner_DigitalDown(void*, unsigned int*, unsigned int, ScannerConfig* cfg, void* Joystick) {
  return ScanPadButton(cfg, Joystick, true) | ScanKey(cfg, true);
}

// ---- a trigger or stick direction bound to a digital event ----------------------------------------------------------

static int ScanPadAxisAsButton(const ScannerConfig* cfg, void* Joystick, bool downOnly) {
  const int  ci   = PadIndex(Joystick);
  const bool down = bGetAnalogDigitalActivationState(ci, cfg->BitmaskStuff, cfg->JoyEvent);
  bool       old  = EventStates[ci][cfg->JoyEvent] != 0;
  const int  ret  = downOnly ? DigitalDownChange(down, old, cfg) : DigitalChange(down, old, cfg, false);
  EventStates[ci][cfg->JoyEvent] = old;
  return ret;
}

static int Scanner_DigitalAnalog(void*, unsigned int*, unsigned int, ScannerConfig* cfg, void* Joystick) {
  return ScanPadAxisAsButton(cfg, Joystick, false) | ScanKey(cfg, false);
}

static int Scanner_DigitalDownAnalog(void*, unsigned int*, unsigned int, ScannerConfig* cfg, void* Joystick) {
  return ScanPadAxisAsButton(cfg, Joystick, true) | ScanKey(cfg, true);
}

// ---- analog axes ---------------------------------------------------------------------------------------------------

// stick/trigger -> the 0-255 byte the game expects (sticks are centered on 0x80)
static bool GetAxisByte(const XINPUT_GAMEPAD& pad, WORD bind, BYTE& axis) {
  auto stick = [](SHORT v) { return (BYTE)((char)(((float)v / (float)0x7FFF) * (float)0x7F) + 0x80); };
  switch (bind) {
    case XINPUT_GAMEPAD_LT_CONFIGDEF:
      axis = pad.bLeftTrigger;
      return true;
    case XINPUT_GAMEPAD_RT_CONFIGDEF:
      axis = pad.bRightTrigger;
      return true;
    case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
      axis = stick(pad.sThumbLX);
      return true;
    case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
      axis = stick(pad.sThumbRX);
      return true;
    case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
      axis = stick(pad.sThumbLY);
      return true;
    case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
      axis = stick(pad.sThumbRY);
      return true;
  }
  return false;
}

static int Scanner_Analog(void*, unsigned int*, unsigned int, ScannerConfig* cfg, void* Joystick) {
  const int ci   = PadIndex(Joystick);
  BYTE      axis = 0;
  GetAxisByte(g_Controllers[ci].state.Gamepad, cfg->BitmaskStuff, axis);

  // a few axes are inverted in the game's own mapping
  switch (cfg->JoyEvent) {
    case JOY_EVENT_DEBUG_CAMERA_INOUT:
    case JOY_EVENT_CARSEL_ORBIT_UPDOWN:
#ifdef GAME_UG2
    case JOY_EVENT_HYDRAULIC_PRESSURIZE_ANALOG_UD:
    case JOY_EVENT_HYDRAULIC_BOUNCE_ANALOG_UD:
#endif
      axis = (BYTE)-axis;
      break;
  }

  // throttle/brake report every call (time sensitive), everything else only on change
  if (axis != EventStates[ci][cfg->JoyEvent] || IsAlwaysReportedEvent(cfg->JoyEvent)) {
    EventStates[ci][cfg->JoyEvent] = axis;
    return cfg->JoyEvent + (axis << 8);
  }
  return 0;
}

// drag race lane change from a stick: only reports the side that matches the event
static int Scanner_Analog_DragSteer(void*, unsigned int*, unsigned int, ScannerConfig* cfg, void* Joystick) {
  const int ci = PadIndex(Joystick);
  BYTE      axis;
  if (cfg->BitmaskStuff == XINPUT_GAMEPAD_LT_CONFIGDEF || cfg->BitmaskStuff == XINPUT_GAMEPAD_RT_CONFIGDEF || !GetAxisByte(g_Controllers[ci].state.Gamepad, cfg->BitmaskStuff, axis)) return 0;

  axis                         = (BYTE)(axis - 0x80);  // back to the signed stick range
  const char signed_axis       = (char)axis;
  if (axis == EventStates[ci][cfg->JoyEvent]) return 0;
  EventStates[ci][cfg->JoyEvent] = axis;
  if (cfg->JoyEvent == JOY_EVENT_DRAG_RACE_CHANGE_LANE_LEFT_ANALOG && signed_axis < 0) return cfg->JoyEvent + (axis << 8);
  if (cfg->JoyEvent == JOY_EVENT_DRAG_RACE_CHANGE_LANE_RIGHT_ANALOG && signed_axis > 0) return cfg->JoyEvent + (axis << 8);
  return 0;
}

// ---- steering ------------------------------------------------------------------------------------------------------

// replaces RealDriver's joystick steering handler: analog axes, pad buttons and keys all add up into one -1..1 value
void RealDriver_JoyHandler_Steer(int JoystickPort, int JoyEvent, char Value, void* RealDriver) {
  float* SteerVal            = (float*)((int)RealDriver + REALDRIVER_STEER_OFFSET);
  float  AnalogSteerValue    = GetAnalogStickValue(JoystickPort, SteerLeftAxis) - GetAnalogStickValue(JoystickPort, SteerRightAxis);
  float  DigitalSteerValue   = 0.0f;
  float  DigitalSteerValueKB = 0.0f;

  const WORD wButtons = g_Controllers[JoystickPort].state.Gamepad.wButtons;
  if (wButtons & SteerLeftButton) DigitalSteerValue += 1.0f;
  if (wButtons & SteerRightButton) DigitalSteerValue -= 1.0f;

  if (IsKeyDown(SteerLeftVKey)) DigitalSteerValueKB += 1.0f;
  if (IsKeyDown(SteerRightVKey)) DigitalSteerValueKB -= 1.0f;

  *SteerVal = clampf(AnalogSteerValue + DigitalSteerValue + DigitalSteerValueKB, -1.0f, 1.0f);
}

// always return a value to trigger polling
static int Scanner_CombinedSteering(void*, unsigned int*, unsigned int, ScannerConfig* cfg, void*) { return cfg->JoyEvent + (1 << 8); }

static int Scanner_TypeChanged(void*, unsigned int*, unsigned int, ScannerConfig*, void*) { return 0; }

// UG2 looks scanner configs up by event and its own lookup returns nothing for the events we add
ScannerConfig* FindScannerConfig_Custom(unsigned int inJoyEvent, int confignum, int unk) {
  for (unsigned int i = 0; i < MAX_JOY_EVENT; i++)
    if (ScannerConfigs[i].JoyEvent == inJoyEvent) return &(ScannerConfigs[i]);
  return 0;
}

void DummyFunc() {}

// ---- INI -> scanner table ------------------------------------------------------------------------------------------

// value with an optional trailing ; comment and surrounding blanks removed
static std::string ReadEventIni(mINI::INIStructure& ini, const char* section, const char* key, const char* fallback = "") {
  if (!ini.has(section) || !ini[section].has(key)) return fallback;
  std::string value = ini[section].get(key);
  value             = value.substr(0, value.find(';'));
  value.erase(value.find_last_not_of(" \t") + 1);
  value.erase(0, value.find_first_not_of(" \t"));
  return value;
}

// VK_* name or a single character
static unsigned int ParseKeyName(const std::string& name) {
  unsigned int vk = ConvertVKNameToValue(name);
  if (vk == 0 && name.length() == 1) vk = (unsigned int)toupper((unsigned char)name[0]);
  return vk;
}

void SetupScannerConfig() {
  mINI::INIFile      inifile(cfgIniName);
  mINI::INIStructure ini;
  inifile.read(ini);

  for (unsigned int i = 0; i < MAX_JOY_EVENT; i++) {
    ScannerConfig& cfg = ScannerConfigs[i];
    cfg.JoyEvent       = i;
    cfg.param          = 0xFF;
    cfg.keycode        = ParseKeyName(ReadEventIni(ini, "EventsKB", JoyEventNames[i]));

    const std::string padBinding = ReadEventIni(ini, "Events", JoyEventNames[i]);
    const int         axisDef    = ConvertXInputOtherConfigDef(padBinding);
    if (!axisDef) {
      cfg.ScannerFunctionPointer = (unsigned int)(bIsEventDigitalDownOnly(i) ? &Scanner_DigitalDown : &Scanner_DigitalUpOrDown);
      cfg.BitmaskStuff           = ConvertXInputNameToBitmask(padBinding);
    } else {
      // triggers or sticks
      cfg.BitmaskStuff = (uint16_t)axisDef;
      if (bIsEventAnalog(i))
        cfg.ScannerFunctionPointer = (unsigned int)&Scanner_Analog;
      else
        cfg.ScannerFunctionPointer = (unsigned int)(bIsEventDigitalDownOnly(i) ? &Scanner_DigitalDownAnalog : &Scanner_DigitalAnalog);
      // analog steering across lanes is broken in drag races, this limits it to the matching side
      if (i == JOY_EVENT_DRAG_RACE_CHANGE_LANE_LEFT_ANALOG || i == JOY_EVENT_DRAG_RACE_CHANGE_LANE_RIGHT_ANALOG) cfg.ScannerFunctionPointer = (unsigned int)&Scanner_Analog_DragSteer;
    }

    if (i == JOY_EVENT_STEER || i == JOY_EVENT_STEER_ANALOG) cfg.ScannerFunctionPointer = (unsigned int)&Scanner_CombinedSteering;
  }

  ScannerConfigs[JOY_EVENT_TYPE_CHANGED].ScannerFunctionPointer = (unsigned int)&Scanner_TypeChanged;
#ifdef GAME_UG2
  ScannerConfigs[JOY_EVENT_LIVE_TYPE_CHANGED].ScannerFunctionPointer = (unsigned int)&Scanner_TypeChanged;
#endif

  SteerLeftButton  = (WORD)ConvertXInputNameToBitmask(ReadEventIni(ini, "Events", "SteerLeftButton"));
  SteerRightButton = (WORD)ConvertXInputNameToBitmask(ReadEventIni(ini, "Events", "SteerRightButton"));
  SteerLeftAxis    = (WORD)ConvertXInputOtherConfigDef(ReadEventIni(ini, "Events", "SteerLeftAxis", "XINPUT_GAMEPAD_LS_LEFT"));
  SteerRightAxis   = (WORD)ConvertXInputOtherConfigDef(ReadEventIni(ini, "Events", "SteerRightAxis", "XINPUT_GAMEPAD_LS_RIGHT"));
  SteerLeftVKey    = (USHORT)ParseKeyName(ReadEventIni(ini, "EventsKB", "KeyboardSteerLeft", "VK_LEFT"));
  SteerRightVKey   = (USHORT)ParseKeyName(ReadEventIni(ini, "EventsKB", "KeyboardSteerRight", "VK_RIGHT"));
}

// the bindings of an event, for the front end button hints
unsigned int UndergroundPadBinding(unsigned int joyEvent) { return joyEvent < MAX_JOY_EVENT ? ScannerConfigs[joyEvent].BitmaskStuff : 0; }
unsigned int UndergroundKeyBinding(unsigned int joyEvent) { return joyEvent < MAX_JOY_EVENT ? ScannerConfigs[joyEvent].keycode : 0; }

// is this event's button/trigger/key held right now (what the scanners last saw)
bool UndergroundEventHeld(int joy, unsigned int joyEvent) {
  if (joy < 0 || joy > 1 || joyEvent >= MAX_JOY_EVENT) return false;
  const ScannerConfig& cfg = ScannerConfigs[joyEvent];
  if (bIsEventAnalog(joyEvent)) {
    // analog events keep the axis byte: only triggers are meaningful here (sticks rest at 0x80)
    if (cfg.BitmaskStuff == XINPUT_GAMEPAD_LT_CONFIGDEF || cfg.BitmaskStuff == XINPUT_GAMEPAD_RT_CONFIGDEF) return EventStates[joy][joyEvent] > 40;
    return false;
  }
  return EventStates[joy][joyEvent] != 0 || (joy == 0 && EventStatesKB[0][joyEvent]);
}

// which right stick axes are already bound to a driving action (shifting, nitro, ...), the chase camera leaves those alone.
// The hydraulics and drag race lane bindings are ignored: they are for special cars/races and would block the camera everywhere
void UndergroundRightStickInUse(bool& horizontal, bool& vertical, bool& click) {
  horizontal = vertical = click = false;
  static const unsigned int events[] = {JOY_EVENT_SHIFTUP,           JOY_EVENT_SHIFTDOWN,             JOY_EVENT_SHIFTUP_ALTERNATE, JOY_EVENT_SHIFTDOWN_ALTERNATE,
                                        JOY_EVENT_NITRO_BOOST,       JOY_EVENT_NITRO_BOOST_ALTERNATE, JOY_EVENT_EBRAKE,            JOY_EVENT_CAMERA_LOOKBACK,
                                        JOY_EVENT_CAMERA_POV_CHANGE, JOY_EVENT_RESET_CAR,             JOY_EVENT_HONK_HORN,         JOY_EVENT_PAUSE};
  for (unsigned int ev : events) {
    switch (ScannerConfigs[ev].BitmaskStuff) {
      case XINPUT_GAMEPAD_RIGHT_THUMB:
        click = true;
        break;
      case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
      case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
      case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
        horizontal = true;
        break;
      case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
      case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
      case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
        vertical = true;
        break;
    }
  }
}

// ---- game side glue ------------------------------------------------------------------------------------------------

// the game's own joystick init, run before we raise the device count it would otherwise reset
static void (*InitJoystick)() = (void (*)())INITJOY_ADDR;

void InitCustomKBInput() {
  InitJoystick();
  *(int*)DEVICE_COUNT_ADDR = 2;  // without this the game doesn't read anything
}

// after each poll: tell the game which devices exist (player 1 is the pad or the keyboard, pad 2 is optional)
void UpdateUndergroundDevices() {
  *(int*)DEVICE_COUNT_ADDR              = g_Controllers[0].bConnected ? 2 : 1;
  *(unsigned char*)JOYSTICKTYPE_P1_ADDR = 1;
  *(unsigned char*)JOYSTICKTYPE_P2_ADDR = g_Controllers[1].bConnected ? 0 : 0xFF;
}

#ifdef GAME_UG
// UG1 reads its scanner table through a pointer: ours takes its place, and the game is told how many entries it has
void InstallUndergroundScannerTable() {
  injector::WriteMemory<unsigned int>(SCANNERCONFIG_POINTER_ADDR, (unsigned int)ScannerConfigs, true);
  *(int*)NUMSCANNERCONFIGS_ADDR = MAX_JOY_EVENT;
}
#endif
#endif  // GAME_UNDERGROUND
