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
#include "plugin/Plugin.h"

// SDL3 is only used for its gamepad API. The headers give us the types and enums; the functions are looked up in
// SDL3.dll at runtime so the plugin still works (on XInput) when the DLL isn't there.
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_gamepad.h>

namespace {

constexpr int   MAX_SLOTS             = 4;
constexpr DWORD DEVICE_RESCAN_MS      = 500;

struct SDLApi {
  bool (SDLCALL* Init)(SDL_InitFlags flags);
  bool (SDLCALL* SetHint)(const char* name, const char* value);
  const char* (SDLCALL* GetError)(void);
  void (SDLCALL* free)(void* mem);
  SDL_JoystickID* (SDLCALL* GetGamepads)(int* count);
  SDL_Gamepad* (SDLCALL* OpenGamepad)(SDL_JoystickID id);
  void (SDLCALL* CloseGamepad)(SDL_Gamepad* pad);
  bool (SDLCALL* GamepadConnected)(SDL_Gamepad* pad);
  bool (SDLCALL* GetGamepadButton)(SDL_Gamepad* pad, SDL_GamepadButton button);
  Sint16 (SDLCALL* GetGamepadAxis)(SDL_Gamepad* pad, SDL_GamepadAxis axis);
  SDL_GamepadType (SDLCALL* GetGamepadType)(SDL_Gamepad* pad);
  const char* (SDLCALL* GetGamepadName)(SDL_Gamepad* pad);
  void (SDLCALL* UpdateGamepads)(void);
  void (SDLCALL* SetGamepadEventsEnabled)(bool enabled);
  bool (SDLCALL* SendGamepadEffect)(SDL_Gamepad* pad, const void* data, int size);  // optional
  bool (SDLCALL* RumbleGamepad)(SDL_Gamepad* pad, Uint16 low, Uint16 high, Uint32 durationMs);  // optional
};

bool LoadApi(HMODULE dll, SDLApi& api) {
  bool ok = true;
  auto get = [&](auto& fn, const char* name) {
    fn = reinterpret_cast<std::remove_reference_t<decltype(fn)>>(GetProcAddress(dll, name));
    if (!fn) {
      Log("SDL: missing export %s", name);
      ok = false;
    }
  };
  get(api.Init, "SDL_Init");
  get(api.SetHint, "SDL_SetHint");
  get(api.GetError, "SDL_GetError");
  get(api.free, "SDL_free");
  get(api.GetGamepads, "SDL_GetGamepads");
  get(api.OpenGamepad, "SDL_OpenGamepad");
  get(api.CloseGamepad, "SDL_CloseGamepad");
  get(api.GamepadConnected, "SDL_GamepadConnected");
  get(api.GetGamepadButton, "SDL_GetGamepadButton");
  get(api.GetGamepadAxis, "SDL_GetGamepadAxis");
  get(api.GetGamepadType, "SDL_GetGamepadType");
  get(api.GetGamepadName, "SDL_GetGamepadName");
  get(api.UpdateGamepads, "SDL_UpdateGamepads");
  get(api.SetGamepadEventsEnabled, "SDL_SetGamepadEventsEnabled");
  // only needed for adaptive triggers: missing is not an error
  api.SendGamepadEffect = reinterpret_cast<decltype(api.SendGamepadEffect)>(GetProcAddress(dll, "SDL_SendGamepadEffect"));
  api.RumbleGamepad     = reinterpret_cast<decltype(api.RumbleGamepad)>(GetProcAddress(dll, "SDL_RumbleGamepad"));
  return ok;
}

HMODULE LoadSDLDll() {
  // scripts\SDL3.dll next to the plugin first (the working directory is the game folder), then the usual search path
  wchar_t dir[MAX_PATH];
  DWORD   len = GetCurrentDirectoryW(MAX_PATH, dir);
  if (len > 0 && len < MAX_PATH - 20) {
    std::wstring path = std::wstring(dir) + L"\\scripts\\SDL3.dll";
    if (HMODULE h = LoadLibraryExW(path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH)) return h;
  }
  return LoadLibraryW(L"SDL3.dll");
}

WORD ButtonMask(SDLApi& api, SDL_Gamepad* pad) {
  struct Map {
    SDL_GamepadButton sdl;
    WORD              xinput;
  };
  static const Map map[] = {
      {SDL_GAMEPAD_BUTTON_SOUTH, XINPUT_GAMEPAD_A},
      {SDL_GAMEPAD_BUTTON_EAST, XINPUT_GAMEPAD_B},
      {SDL_GAMEPAD_BUTTON_WEST, XINPUT_GAMEPAD_X},
      {SDL_GAMEPAD_BUTTON_NORTH, XINPUT_GAMEPAD_Y},
      {SDL_GAMEPAD_BUTTON_BACK, XINPUT_GAMEPAD_BACK},
      {SDL_GAMEPAD_BUTTON_START, XINPUT_GAMEPAD_START},
      {SDL_GAMEPAD_BUTTON_LEFT_STICK, XINPUT_GAMEPAD_LEFT_THUMB},
      {SDL_GAMEPAD_BUTTON_RIGHT_STICK, XINPUT_GAMEPAD_RIGHT_THUMB},
      {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, XINPUT_GAMEPAD_LEFT_SHOULDER},
      {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER},
      {SDL_GAMEPAD_BUTTON_DPAD_UP, XINPUT_GAMEPAD_DPAD_UP},
      {SDL_GAMEPAD_BUTTON_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_DOWN},
      {SDL_GAMEPAD_BUTTON_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_LEFT},
      {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, XINPUT_GAMEPAD_DPAD_RIGHT},
  };
  WORD mask = 0;
  for (const Map& m : map)
    if (api.GetGamepadButton(pad, m.sdl)) mask |= m.xinput;
  return mask;
}

// SDL sticks are +down, XInput sticks are +up
SHORT StickAxis(SDLApi& api, SDL_Gamepad* pad, SDL_GamepadAxis axis, bool invert) {
  Sint16 v = api.GetGamepadAxis(pad, axis);
  if (!invert) return v;
  return v == -32768 ? 32767 : (SHORT)-v;
}

// SDL triggers are 0..32767, XInput triggers 0..255
BYTE TriggerAxis(SDLApi& api, SDL_Gamepad* pad, SDL_GamepadAxis axis) {
  Sint16 v = api.GetGamepadAxis(pad, axis);
  return v <= 0 ? 0 : (BYTE)((v * 255 + 16383) / 32767);
}

int IconModeForType(SDL_GamepadType type) {
  switch (type) {
    case SDL_GAMEPAD_TYPE_XBOX360:
      return CONTROLLERICON_XBOX360;
    case SDL_GAMEPAD_TYPE_XBOXONE:
      return CONTROLLERICON_XBOXONE;
    case SDL_GAMEPAD_TYPE_PS3:
      return CONTROLLERICON_PS3;
    case SDL_GAMEPAD_TYPE_PS4:
    case SDL_GAMEPAD_TYPE_PS5:
      return CONTROLLERICON_PS4;
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
      return CONTROLLERICON_SWITCH;
    default:
      return -1;
  }
}

class SDLBackend : public GamepadBackend {
 public:
  bool Init() {
    HMODULE dll = LoadSDLDll();
    if (!dll) {
      Log("SDL: SDL3.dll not found (expected in scripts\\)");
      return false;
    }
    if (!LoadApi(dll, m_Api)) return false;

    // no window or event loop here: let SDL poll devices on its own thread and read state directly
    m_Api.SetHint(SDL_HINT_JOYSTICK_THREAD, "1");
    m_Api.SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (!gSDLIgnoreDevices.empty()) {
      m_Api.SetHint(SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES, gSDLIgnoreDevices.c_str());
      Log("SDL: ignoring devices %s", gSDLIgnoreDevices.c_str());
    }
    if (!m_Api.Init(SDL_INIT_GAMEPAD)) {
      Log("SDL: SDL_Init failed: %s", m_Api.GetError());
      return false;
    }
    m_Api.SetGamepadEventsEnabled(false);
    Rescan();
    return true;
  }

  const char* Name() const override { return "SDL"; }

  void BeginFrame() override {
    m_Api.UpdateGamepads();
    DWORD now = GetTickCount();
    if (now - m_LastScan >= DEVICE_RESCAN_MS) Rescan();
  }

  bool Poll(int slot, XINPUT_STATE& state) override {
    if (slot < 0 || slot >= MAX_SLOTS) return false;
    SDL_Gamepad* pad = m_Pads[slot];
    if (!pad || !m_Api.GamepadConnected(pad)) return false;

    XINPUT_GAMEPAD& g = state.Gamepad;
    g.wButtons        = ButtonMask(m_Api, pad);
    g.sThumbLX        = StickAxis(m_Api, pad, SDL_GAMEPAD_AXIS_LEFTX, false);
    g.sThumbLY        = StickAxis(m_Api, pad, SDL_GAMEPAD_AXIS_LEFTY, true);
    g.sThumbRX        = StickAxis(m_Api, pad, SDL_GAMEPAD_AXIS_RIGHTX, false);
    g.sThumbRY        = StickAxis(m_Api, pad, SDL_GAMEPAD_AXIS_RIGHTY, true);
    g.bLeftTrigger    = TriggerAxis(m_Api, pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    g.bRightTrigger   = TriggerAxis(m_Api, pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
    state.dwPacketNumber++;
    return true;
  }

  // DualSense output report as SDL's HIDAPI driver takes it (47 bytes): enable bits, motors, audio, then the two 11-byte
  // trigger effect blocks (right at offset 10, left at 21), then LED data we leave untouched.
  bool SetTriggerEffects(int slot, const uint8_t left[11], const uint8_t right[11]) override {
    if (!m_Api.SendGamepadEffect || slot < 0 || slot >= MAX_SLOTS || !m_Pads[slot]) return false;
    if (m_Api.GetGamepadType(m_Pads[slot]) != SDL_GAMEPAD_TYPE_PS5) return false;

    uint8_t state[47] = {};
    state[0]          = 0x04 | 0x08;  // right + left trigger effect valid
    memcpy(state + 10, right, 11);
    memcpy(state + 21, left, 11);
    const bool ok = m_Api.SendGamepadEffect(m_Pads[slot], state, (int)sizeof(state));
    if (!ok && !m_EffectErrorLogged) {
      m_EffectErrorLogged = true;
      Log("SDL: sending the trigger effect failed: %s", m_Api.GetError());
    }
    return ok;
  }

  bool Rumble(int slot, uint16_t low, uint16_t high, uint32_t durationMs) override {
    if (!m_Api.RumbleGamepad || slot < 0 || slot >= MAX_SLOTS || !m_Pads[slot]) return false;
    return m_Api.RumbleGamepad(m_Pads[slot], low, high, durationMs);
  }

  int IconModeForSlot(int slot) override {
    if (slot < 0 || slot >= MAX_SLOTS || !m_Pads[slot]) return -1;
    return IconModeForType(m_Api.GetGamepadType(m_Pads[slot]));
  }

 private:
  // pads keep their slot for as long as they stay plugged in: unplugging one frees only its own slot, and a newly
  // plugged pad takes the first free slot, so player 1 never changes because of another pad coming or going
  void Rescan() {
    m_LastScan = GetTickCount();
    int             count = 0;
    SDL_JoystickID* ids   = m_Api.GetGamepads(&count);
    if (!ids) return;

    for (int i = 0; i < MAX_SLOTS; i++) {
      if (!m_Pads[i]) continue;
      bool present = false;
      for (int j = 0; j < count && !present; j++) present = (ids[j] == m_Ids[i]);
      if (present) continue;
      m_Api.CloseGamepad(m_Pads[i]);
      m_Pads[i] = NULL;
      m_Ids[i]  = 0;
      Log("SDL: slot %d freed", i);
    }

    for (int j = 0; j < count; j++) {
      bool known = false;
      for (int i = 0; i < MAX_SLOTS && !known; i++) known = (m_Pads[i] && m_Ids[i] == ids[j]);
      if (known) continue;
      for (int i = 0; i < MAX_SLOTS; i++) {
        if (m_Pads[i]) continue;
        m_Pads[i] = m_Api.OpenGamepad(ids[j]);
        m_Ids[i]  = ids[j];
        Log("SDL: slot %d = %s", i, m_Pads[i] ? m_Api.GetGamepadName(m_Pads[i]) : "(open failed)");
        break;
      }
    }
    m_Api.free(ids);
  }

  SDLApi         m_Api            = {};
  SDL_Gamepad*   m_Pads[MAX_SLOTS] = {};
  SDL_JoystickID m_Ids[MAX_SLOTS]  = {};
  DWORD          m_LastScan        = 0;
  bool           m_EffectErrorLogged = false;
};

}  // namespace

GamepadBackend* CreateSDLBackend() {
  SDLBackend* backend = new SDLBackend();
  if (!backend->Init()) {
    delete backend;
    return NULL;
  }
  return backend;
}
