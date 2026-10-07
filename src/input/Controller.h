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
#include <XInput.h>
#pragma comment(lib, "xinput.lib")

constexpr int MAX_CONTROLLERS = 4;  // XInput handles up to 4 controllers
constexpr SHORT SHORT_MAX = std::numeric_limits<SHORT>::max();

constexpr SHORT PercentToAxis(float percent) { return static_cast<SHORT>(percent * SHORT_MAX); }
// triggers are 0-255, not the thumbstick range
constexpr BYTE PercentToTrigger(float percent) { return static_cast<BYTE>(percent * 255.0f); }

// true: circular deadzone, output rescaled to the full range (fixes the 0.75 diagonal). false: legacy per-axis cut
extern bool bRadialDeadzone;
extern SHORT INPUT_DEADZONE_LS;
extern SHORT INPUT_DEADZONE_RS;
extern SHORT INPUT_DEADZONE_LS_P2;
extern SHORT INPUT_DEADZONE_RS_P2;
extern SHORT SHIFT_ANALOG_THRESHOLD;
extern SHORT FEUPDOWN_ANALOG_THRESHOLD;
extern BYTE TRIGGER_ACTIVATION_THRESHOLD;

// for triggering the over-zelaous inputs once in a tick...
extern WORD bQuitButtonOldState;

struct CONTROLLER_STATE {
  XINPUT_STATE state;
  bool         bConnected;
};

extern CONTROLLER_STATE g_Controllers[MAX_CONTROLLERS];
extern bool bXInputOmniMode;
extern bool bPassConnStatus;

// KB Input declarations
extern BYTE VKeyStates[2][256];
extern BYTE AsyncKeyCache[256];  // 0 = not read this poll, 1 = up, 2 = down
extern unsigned int KeyboardReadingMode;
extern bool bMouseLook;

// thumbstick -> cursor pixels per tick for the gamepad-driven cursor
constexpr int VIRTUAL_MOUSE_STICK_DIVISOR = 0x1000;
extern bool bEnteredWorldMapOnce;

// XInputGetState is slow on empty slots, so disconnected pads are only re-polled periodically
constexpr ULONGLONG CONTROLLER_REPOLL_INTERVAL_MS = 1000;
extern ULONGLONG g_ControllerNextPoll[MAX_CONTROLLERS];
HRESULT UpdateControllerState();

// old function -- repurposed to send keyboard-exclusive commands to the game
void ReadXInput_Extra();

// unbuffered mode reads each bound key at most once per poll
bool IsAsyncKeyDown(BYTE vkey);
void __stdcall ReadControllerData();
float GetAnalogStickValue(int index, WORD bind);
uint16_t GetAnalogActivity();
bool bGetAnalogDigitalActivationState(int index, WORD bind, int actid);
