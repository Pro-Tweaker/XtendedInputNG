/*
// clang-format off
//
//    Xtended Input NG - native gamepad support for the PC versions of Need for Speed games
//    Continues xan1242's NFS-XtendedInput and NFSU-XtendedInput
//
//    MIT License
//
//    Copyright (c) 2026 Pro-Tweaker
//
// clang-format on
*/

#pragma once
#include <XInput.h>
#include <cstdint>
#include <string>

// Where gamepad state comes from. Everything above this layer (deadzones, bindings, icons) keeps working on
// XINPUT_STATE, so a backend only has to translate its device state into that struct.
class GamepadBackend {
 public:
  virtual ~GamepadBackend() {}
  virtual const char* Name() const = 0;

  // called once per frame before any Poll()
  virtual void BeginFrame() {}

  // fills 'state' for the pad in 'slot' (0-3). false = nothing connected in that slot
  virtual bool Poll(int slot, XINPUT_STATE& state) = 0;

  // CONTROLLERICON_* that matches the pad in 'slot', or -1 when the backend can't tell
  virtual int IconModeForSlot(int slot) { return -1; }

  // DualSense adaptive triggers: sends the two 11-byte effect blocks (see TriggerEffects.h). false = not supported by this pad/backend
  virtual bool SetTriggerEffects(int slot, const uint8_t left[11], const uint8_t right[11]) { return false; }

  // Rumble: low = big (low frequency) motor, high = small (high frequency) motor, 0-65535. It stops by itself after durationMs
  virtual bool Rumble(int slot, uint16_t low, uint16_t high, uint32_t durationMs) { return false; }
};

// Selected with "Backend = XInput|SDL" in the [Input] section. SDL needs scripts\SDL3.dll and falls back to XInput
// when it is missing. Nothing is loaded before the first frame is polled (loading DLLs in DllMain is not allowed).
extern std::string gGamepadBackendName;

// "IgnoreDevices" in [Input]: comma separated VID/PID pairs (e.g. 0x045e/0x028e) that the SDL backend must not open. Hides the
// virtual Xbox pad of a wrapper (Steam Input, DS4Windows) so the real pad isn't seen twice. Empty = ignore nothing.
extern std::string gSDLIgnoreDevices;
GamepadBackend*    GetGamepadBackend();

// implemented by SDLBackend.cpp; returns NULL when SDL3.dll can't be used (the reason is logged)
GamepadBackend* CreateSDLBackend();
