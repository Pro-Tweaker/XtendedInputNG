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

#include "stdafx.h"
#include "input/GamepadBackend.h"
#include "plugin/Log.h"

std::string gGamepadBackendName = "SDL";
std::string gSDLIgnoreDevices;

namespace {

class XInputBackend : public GamepadBackend {
 public:
  const char* Name() const override { return "XInput"; }
  bool        Poll(int slot, XINPUT_STATE& state) override { return XInputGetState(slot, &state) == ERROR_SUCCESS; }
  bool        Rumble(int slot, uint16_t low, uint16_t high, uint32_t) override {
    XINPUT_VIBRATION v = {low, high};  // XInput keeps it running until the next call
    return XInputSetState(slot, &v) == ERROR_SUCCESS;
  }
};

}  // namespace

GamepadBackend* GetGamepadBackend() {
  static GamepadBackend* backend = NULL;
  if (backend) return backend;

  // first poll: runs on the game's own thread, so loading SDL3.dll is safe here
  std::string wanted = gGamepadBackendName;
  std::transform(wanted.begin(), wanted.end(), wanted.begin(), ::tolower);
  if (wanted == "sdl") {
    backend = CreateSDLBackend();
    if (!backend) Log("Backend: SDL is unavailable, falling back to XInput");
  } else if (wanted != "xinput") {
    Log("Backend: unknown value '%s', using XInput", gGamepadBackendName.c_str());
  }
  if (!backend) backend = new XInputBackend();
  Log("Backend: %s", backend->Name());
  return backend;
}
