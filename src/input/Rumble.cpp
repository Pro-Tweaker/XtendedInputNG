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
#include "input/Rumble.h"
#include "game/ActionID.h"
#include "input/Controller.h"
#include "input/GamepadBackend.h"
#include "input/InputDevice.h"
#include "plugin/Log.h"
#include "plugin/PatternScan.h"
#include "plugin/Plugin.h"

RumbleConfig gRumble;

#ifdef GAME_CARBON

namespace {

constexpr DWORD     SEND_INTERVAL_MS      = 100;
constexpr DWORD     RUMBLE_DURATION_MS    = 250;  // each update re-arms it; the pad stops by itself if we stop updating

// Calibrated on a recorded run (speed sampled at 50 Hz): a crash lost 12 speed units in one 20 ms step (about 200 units/s
// averaged over three frames) while the hardest braking and curb bumps stayed below 40 units/s.
constexpr float IMPACT_MIN_AGE_S     = 0.04f;  // compare against a sample at least this old (the poll can run twice per frame)
constexpr float IMPACT_MAX_AGE_S     = 0.12f;
constexpr float IMPACT_MIN_RATE      = 100.0f; // speed units lost per second to count as a hit
constexpr float IMPACT_FULL_RATE     = 260.0f; // ... and the rate that gives the full-strength pulse
constexpr float IMPACT_MIN_SPEED     = 3.0f;   // ignore hits when (almost) standing still
constexpr float IMPACT_REARM_S       = 0.25f;
constexpr float IMPACT_DECAY_PER_SEC = 3.0f;   // the pulse fades out in about a third of a second
constexpr float SPEED_REF_FLOOR      = 30.0f;  // speed that counts as "fast" until the car has gone faster

// pad motors don't turn at all below roughly 15% power: anything noticeable is lifted over that dead zone, and levels too small
// to feel are dropped instead of becoming a constant buzz
float OverDeadZone(float v) { return v < 0.04f ? 0.0f : 0.15f + 0.85f * std::min(v, 1.0f); }

// The game's script function PlayerGetSpeed() is a stub (it pops the real value and returns a constant), but its code shows
// where the player is: a global count, a global player list, and the virtual getter that returns the speed in ST0:
//   mov eax,[count]; test; je ...; mov eax,[list]; mov ecx,[eax]; test; je ...; mov edx,[ecx]; call [edx+slot]; fstp st(0)
// The addresses and the vtable slot are read from those instructions, so no build-specific numbers are hard-coded.
struct PlayerSource {
  bool      resolved = false;
  bool      usable   = false;
  uintptr_t countAddr = 0, listAddr = 0, slotOffset = 0;
  uintptr_t stateAddr = 0;  // the game flow state (6 = racing)
};
PlayerSource g_Player;

// The game flow state is a global the flow manager sets with "mov dword ptr [state], N". The write of 6 (racing) sits in a
// fixed sequence, which identifies the variable exactly:
//   add esp,4; test eax,eax; mov dword ptr [state], 6; je ...
uintptr_t FindGameStateAddr() {
  const uint8_t* p = FindPattern("83 C4 04 85 C0 C7 05 ? ? ? ? 06 00 00 00 74 ?");
  return p ? *(const uint32_t*)(p + 7) : 0;
}

void ResolvePlayerSource() {
  g_Player.resolved = true;
  const uint8_t* p  = FindPattern("A1 ? ? ? ? 85 C0 74 ? A1 ? ? ? ? 8B 08 85 C9 74 ? 8B 11 FF 92 ? ? ? ? DD D8 D9 05 ? ? ? ? C3");
  if (!p) {
    Log("Rumble: the player speed code was not found in this executable, rumble is disabled");
    return;
  }
  g_Player.countAddr  = *(const uint32_t*)(p + 1);
  g_Player.listAddr   = *(const uint32_t*)(p + 10);
  g_Player.slotOffset = *(const uint32_t*)(p + 24);
  g_Player.stateAddr  = FindGameStateAddr();
  if (!g_Player.stateAddr) {
    Log("Rumble: the game state variable was not found in this executable, rumble is disabled");
    return;
  }
  g_Player.usable = true;
  Log("Rumble: player found through the game's code (count at %08X, list at %08X, speed getter in vtable slot +%X), game state at %08X",
      (unsigned)g_Player.countAddr, (unsigned)g_Player.listAddr, (unsigned)g_Player.slotOffset, (unsigned)g_Player.stateAddr);
}

float PlayerSpeed() {
  if (!g_Player.resolved) ResolvePlayerSource();
  if (!g_Player.usable) return 0.0f;
  __try {
    if (!*(int*)g_Player.countAddr) return 0.0f;
    void** list = *(void***)g_Player.listAddr;
    if (!list || !list[0]) return 0.0f;
    void* player = list[0];
    void** vtable = *(void***)player;
    auto  getter = (float(__thiscall*)(void*))vtable[g_Player.slotOffset / sizeof(void*)];
    return getter(player);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return 0.0f;
  }
}

struct History {
  float time[32];
  float speed[32];
  int   head  = 0;
  int   count = 0;
  void  push(float t, float v) {
    time[head]  = t;
    speed[head] = v;
    head        = (head + 1) % 32;
    count       = std::min(count + 1, 32);
  }
  // newest sample that is at least minAge old (and no older than maxAge)
  bool older(float now, float minAge, float maxAge, float& v, float& age) const {
    for (int i = 0; i < count; i++) {
      int   idx = (head - 1 - i + 64) % 32;
      float a   = now - time[idx];
      if (a > maxAge) return false;
      if (a >= minAge) {
        v   = speed[idx];
        age = a;
        return true;
      }
    }
    return false;
  }
};

History g_History;
float   g_Impact     = 0.0f;
float   g_RearmLeft  = 0.0f;
float   g_SpeedRef   = SPEED_REF_FLOOR;
float   g_Clock      = 0.0f;
bool    g_WasRacing  = false;
WORD    g_LastLow    = 0, g_LastHigh = 0;
DWORD   g_LastSend   = 0;
DWORD   g_LastDebug  = 0;

float FrameSeconds() {
  static LARGE_INTEGER freq = {}, last = {};
  if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  float dt = last.QuadPart ? (float)((double)(now.QuadPart - last.QuadPart) / (double)freq.QuadPart) : 1.0f / 60.0f;
  last     = now;
  return std::min(dt, 0.1f);
}

void Send(WORD low, WORD high, bool force) {
  const DWORD now = GetTickCount();
  const bool  changed = std::abs((int)low - (int)g_LastLow) > 1300 || std::abs((int)high - (int)g_LastHigh) > 1300;  // ~2%
  if (!force && !changed && now - g_LastSend < SEND_INTERVAL_MS) return;
  g_LastLow  = low;
  g_LastHigh = high;
  g_LastSend = now;
  const bool ok = GetGamepadBackend()->Rumble(0, low, high, RUMBLE_DURATION_MS);

  static bool loggedFirst = false, loggedFail = false;
  if (!ok && !loggedFail) {
    loggedFail = true;
    Log("Rumble: the %s backend refused the rumble command (low=%u high=%u)", GetGamepadBackend()->Name(), (unsigned)low, (unsigned)high);
  }
  if (ok && (low || high) && !loggedFirst) {
    loggedFirst = true;
    Log("Rumble: first vibration sent through the %s backend (low=%u high=%u)", GetGamepadBackend()->Name(), (unsigned)low, (unsigned)high);
  }
}

}  // namespace

void UpdateRumble() {
  if (!gRumble.enable || gRumble.strength <= 0.0f) return;
  if (!g_Controllers[0].bConnected) return;

  if (!g_Player.resolved) ResolvePlayerSource();
  if (!g_Player.usable) return;
  const bool racing = *(int*)g_Player.stateAddr == 6;
  if (!racing && gRumble.onlyWhileRacing) {
    if (g_WasRacing) {  // leaving the race: stop whatever is still running
      Send(0, 0, true);
      g_WasRacing = false;
      g_Impact    = 0.0f;
      g_History.count = 0;
    }
    return;
  }
  if (!g_WasRacing) Log("Rumble: racing detected (game state %d, speed %.1f)", *(int*)g_Player.stateAddr, PlayerSpeed());
  g_WasRacing = true;

  const float dt = FrameSeconds();
  g_Clock += dt;
  const float speed = std::max(0.0f, PlayerSpeed());
  g_History.push(g_Clock, speed);
  g_SpeedRef = std::max(g_SpeedRef * 0.9999f, std::max(speed, SPEED_REF_FLOOR));

  // ---- impacts: speed lost faster than any braking can
  g_RearmLeft = std::max(0.0f, g_RearmLeft - dt);
  g_Impact    = std::max(0.0f, g_Impact - IMPACT_DECAY_PER_SEC * dt);
  float before, age;
  if (g_RearmLeft <= 0.0f && g_History.older(g_Clock, IMPACT_MIN_AGE_S, IMPACT_MAX_AGE_S, before, age) && before > IMPACT_MIN_SPEED) {
    const float rate = (before - speed) / age;
    if (rate >= IMPACT_MIN_RATE) {
      const float level = 0.35f + 0.65f * std::min(1.0f, (rate - IMPACT_MIN_RATE) / (IMPACT_FULL_RATE - IMPACT_MIN_RATE));
      g_Impact          = std::max(g_Impact, level);
      g_RearmLeft       = IMPACT_REARM_S;
      if (gRumble.debugLog) Log("Rumble: impact, speed %.1f -> %.1f in %.0f ms (%.0f units/s), level %.2f", before, speed, age * 1000.0f, rate, level);
    }
  }

  // ---- road/engine and NOS
  const float speedNorm = std::min(1.0f, speed / g_SpeedRef);
  const float throttle  = std::max(0.0f, std::min(1.0f, CurrValues[0][GAMEACTION_GAS]));
  const float nosHeld   = (CurrValues[0][GAMEACTION_NOS] > 0.5f && speed > 2.0f) ? 1.0f : 0.0f;

  float low  = gRumble.road * speedNorm * (0.3f + 0.7f * throttle) + gRumble.nos * 0.6f * nosHeld + gRumble.impact * g_Impact;
  float high = gRumble.road * 0.4f * speedNorm * speedNorm * throttle + gRumble.nos * nosHeld + gRumble.impact * 0.8f * g_Impact;
  low        = std::min(1.0f, low * gRumble.strength);
  high       = std::min(1.0f, high * gRumble.strength);
  Send((WORD)(OverDeadZone(low) * 65535.0f), (WORD)(OverDeadZone(high) * 65535.0f), false);

  if (gRumble.debugLog) {
    const DWORD now = GetTickCount();
    if (now - g_LastDebug > 1000) {
      g_LastDebug = now;
      Log("Rumble: speed=%.1f ref=%.1f throttle=%.2f nos=%.0f impact=%.2f -> low=%.2f high=%.2f", speed, g_SpeedRef, throttle, nosHeld, g_Impact, low, high);
    }
  }
}

#elif defined(GAME_UG2)

// ---------------------------------------------------------------------------------------------------------------------
// Underground 2
//
// The game has its own force feedback code for wheels (one function, called every tick for each player, works out surface
// rumble, tire slip/bump pulses and steering forces and hands them to a DirectInput effect layer). It never runs here because
// that layer only works when DirectInput found a device, and the plugin turns DirectInput off. So the effect layer itself is
// replaced: the question "is there a device" is answered yes for a connected pad, and the effect calls are caught and kept
// as plain numbers that UpdateRumble() turns into rumble. The game's own logic decides when and how hard.
//
// The layer is only ever called from that one function (checked for every address below), so replacing it is safe. The
// methods are thiscall (this = the input manager), the helpers are plain stdcall.
// ---------------------------------------------------------------------------------------------------------------------
#include <injector/injector.hpp>

namespace {

constexpr DWORD SEND_INTERVAL_MS   = 100;
constexpr DWORD RUMBLE_DURATION_MS = 250;  // each update re-arms it; the pad stops by itself if we stop updating
constexpr DWORD EFFECT_LIVE_MS     = 150;  // the game updates an effect every tick while it applies; silence for this long ends it
constexpr DWORD SHAKE_MS          = 300;   // bumps and hits arrive once, not every tick: the pulse is held and fades out over this time
constexpr float ROAD_FULL          = 6000.0f;   // the road effect's strength at its highest (the game's range is 0..10000)
constexpr float EFFECT_MAX         = 10000.0f;  // the game's force range

struct ForceSlot {
  float road = 0.0f, shake = 0.0f, offroad = 0.0f;  // last values the game asked for
  DWORD roadTick = 0, shakeTick = 0, offroadTick = 0;
  bool  engaged = false;  // the game's "force feedback is on" flag for this player
};
ForceSlot g_Force[2];

// what the car is doing, measured every tick from its velocity
struct DrivingState {
  float speed = 0.0f;     // m/s
  float lateral = 0.0f;   // sideways acceleration, smoothed
  float vx = 0.0f, vy = 0.0f;
  LONGLONG stamp = 0;
  DWORD tick = 0;
  bool  valid = false;
};
DrivingState g_Drive[2];

// how often the game called each part of the layer since the last debug line
struct CallCounts {
  unsigned ticks = 0, ready = 0, road = 0, shake = 0, offroad = 0, engage = 0, disengage = 0, stopRoad = 0, stopAll = 0;
};
CallCounts g_Calls;
WORD      g_LastLow[2] = {0, 0}, g_LastHigh[2] = {0, 0};
DWORD     g_LastSend[2] = {0, 0};
DWORD     g_LastDebug   = 0;
void*     g_ManagerStub = NULL;

bool Usable(int joy) { return joy >= 0 && joy < 2 && gRumble.enable && gRumble.strength > 0.0f && g_Controllers[joy].bConnected; }

// logs the first call of each kind once, so the log shows which parts of the game's force feedback code run
void LogFirstCall(const char* what, int joy, float value) {
  static const char* seen[8];
  static int         count = 0;
  for (int i = 0; i < count; i++)
    if (seen[i] == what) return;
  if (count < 8) {
    seen[count++] = what;
    Log("Rumble: the game's force feedback code is running (first %s call, pad %d, value %.0f)", what, joy, value);
  }
}

// ---- the replaced effect layer ---------------------------------------------------------------------------------------

// stdcall helpers: is there a device / does it support an effect type
BOOL __stdcall FF_DeviceReady(int joy) {
  g_Calls.ready++;
  return Usable(joy) ? 1 : 0;
}
BOOL __stdcall FF_HasCapability(int joy, int type) { return 1; }  // type 2 = "periodic effects": the game then skips its wheel-only steering force
void __stdcall FF_StopSteering(int joy) {}
void __stdcall FF_StopSecondary(int joy) { g_Calls.stopAll++; }

// thiscall methods: this in ecx, the rest on the stack and popped by the callee, which is what __fastcall does with two register slots
bool __fastcall FF_IsEffectActive(void*, void*, int joy, int id) { return id == 9 && joy >= 0 && joy < 2 && g_Force[joy].engaged; }

void __fastcall FF_Spring(void*, void*, int joy, int a, int b, int c) {}
void __fastcall FF_ConstantForce(void*, void*, int joy, int magnitude, int duration) {}
void __fastcall FF_Damper(void*, void*, int joy, int value) {}

// short pulse on bumps and tire slip, fed every tick while it lasts
void __fastcall FF_Shake(void*, void*, int joy, int value) {
  g_Calls.shake++;
  if (joy < 0 || joy > 1) return;
  g_Force[joy].shake     = (float)value;
  g_Force[joy].shakeTick = GetTickCount();
  LogFirstCall("shake", joy, (float)value);
}

// the strongest effect, used on some surfaces
void __fastcall FF_Offroad(void*, void*, int joy, int value) {
  g_Calls.offroad++;
  if (joy < 0 || joy > 1) return;
  g_Force[joy].offroad     = (float)value;
  g_Force[joy].offroadTick = GetTickCount();
  LogFirstCall("offroad", joy, (float)value);
}

// road surface rumble: waveform 0-2, strength and a period
void __fastcall FF_Road(void*, void*, int joy, int waveform, int magnitude, int period) {
  g_Calls.road++;
  if (joy < 0 || joy > 1) return;
  g_Force[joy].road     = (float)magnitude;
  g_Force[joy].roadTick = GetTickCount();
  LogFirstCall("road", joy, (float)magnitude);
}

// the game calls this every tick it is not asking for the offroad effect: it stops that one, the road effect has its own value
void __fastcall FF_StopRoad(void*, void*, int joy) {
  g_Calls.stopRoad++;
  if (joy < 0 || joy > 1) return;
  g_Force[joy].offroad = 0.0f;
}

void __fastcall FF_Nothing(void*, void*, int joy) {}

void __fastcall FF_Engage(void*, void*, int joy) {
  g_Calls.engage++;
  if (joy < 0 || joy > 1) return;
  g_Force[joy].engaged = true;
  LogFirstCall("engage", joy, 0.0f);
}

void __fastcall FF_Disengage(void*, void*, int joy) {
  g_Calls.disengage++;
  if (joy < 0 || joy > 1) return;
  g_Force[joy] = ForceSlot();
}

// The game's per-tick force feedback function. Only wrapped to see what it works with: it runs the same either way
typedef void(__thiscall* ForceTick_t)(void* self, float steer, int joy);

template <typename T>
bool SafeRead(const void* address, T& out) {
  __try {
    out = *(const T*)address;
    return true;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

void LogForceTick(void* self, float steer, int joy) {
  static DWORD last = 0;
  const DWORD  now  = GetTickCount();
  if (now - last < 1000) return;
  last = now;

  uintptr_t vehicle = 0, descriptor = 0, state = 0, stateValue = 0;
  int       vflag = 0, pauseFlag = 0;
  float     slip[4] = {};
  BYTE      f390 = 0, f391 = 0;
  int       surfaceA = 0, surfaceB = 0;
  SafeRead(self, vehicle);
  if (vehicle) {
    SafeRead((const void*)(vehicle + 0x28), vflag);
    SafeRead((const void*)(vehicle + 0x18), descriptor);
    for (int i = 0; i < 4; i++) SafeRead((const void*)(vehicle + 0x370 + i * 4), slip[i]);
    SafeRead((const void*)(vehicle + 0x390), f390);
    SafeRead((const void*)(vehicle + 0x391), f391);
    SafeRead((const void*)(vehicle + 0x394), surfaceA);
    SafeRead((const void*)(vehicle + 0x398), surfaceB);
  }
  SafeRead((const void*)0x008900D8, state);
  if (state) SafeRead((const void*)state, stateValue);
  SafeRead((const void*)0x00890108, pauseFlag);
  Log("Rumble: tick joy=%d steer=%.2f vehicle=%08X [+28]=%d [+18]=%08X slip=%.2f/%.2f/%.2f/%.2f flags=%d/%d surface=%d/%d state=%08X->%d pause=%d", joy, steer, (unsigned)vehicle, vflag,
      (unsigned)descriptor, slip[0], slip[1], slip[2], slip[3], (int)f390, (int)f391, surfaceA, surfaceB, (unsigned)state, (int)stateValue, pauseFlag);
  if (joy >= 0 && joy < 2) Log("Rumble: car speed=%.1f m/s (%.0f km/h) sideways=%.1f m/s2", g_Drive[joy].speed, g_Drive[joy].speed * 3.6f, g_Drive[joy].lateral);
  Log("Rumble: calls/s ticks=%u ready=%u road=%u shake=%u offroad=%u engage=%u disengage=%u stopRoad=%u stopAll=%u", g_Calls.ticks, g_Calls.ready, g_Calls.road, g_Calls.shake, g_Calls.offroad,
      g_Calls.engage, g_Calls.disengage, g_Calls.stopRoad, g_Calls.stopAll);
  g_Calls = CallCounts();
}

struct Vec3f {
  float x, y, z;
};

// ---- nitrous level ---------------------------------------------------------------------------------------------------
// Where the car keeps its nitrous is not known, so by default the nitro rumble follows the button alone. Setting NosField in
// [Rumble] to the place of a float in the vehicle ("0x3C4", or "0x40:0x3C4" for a float inside an object the vehicle points to)
// makes it rumble only while that float is above zero.
struct NosField {
  int pointerOffset = -1;  // -1: the float is in the vehicle itself
  int offset        = -1;  // -1: not set
  bool Known() const { return offset >= 0; }
};
NosField g_NosField;
float    g_NosLevel = -1.0f;  // -1: unknown
float    g_NosFull  = 0.0f;   // the highest level seen, for the "is it empty" test

bool ReadNosLevel(uintptr_t vehicle, float& level) {
  if (!g_NosField.Known()) return false;
  uintptr_t base = vehicle;
  if (g_NosField.pointerOffset >= 0 && (!SafeRead((const void*)(vehicle + g_NosField.pointerOffset), base) || base < 0x10000)) return false;
  return SafeRead((const void*)(base + g_NosField.offset), level) && std::isfinite(level);
}

// speed and sideways acceleration from the car's velocity vector (the game itself takes its length for its wheel force)
void UpdateDriving(void* self, int joy) {
  if (joy < 0 || joy > 1) return;
  uintptr_t vehicle = 0;
  Vec3f     v       = {};
  if (!SafeRead(self, vehicle) || !vehicle || !SafeRead((const void*)(vehicle + 0x70), v)) return;
  if (!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z)) return;

  static LARGE_INTEGER freq = {};
  if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);

  DrivingState& d  = g_Drive[joy];
  const float   dt = d.stamp ? (float)((double)(now.QuadPart - d.stamp) / (double)freq.QuadPart) : 0.0f;
  d.speed          = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (d.valid && dt > 0.004f && dt < 0.1f) {
    // acceleration across the direction of travel: |a x v| / |v| on the ground plane
    const float horizontal = std::sqrt(v.x * v.x + v.y * v.y);
    if (horizontal > 5.0f) {
      const float ax = (v.x - d.vx) / dt, ay = (v.y - d.vy) / dt;
      const float lateral = std::fabs(ax * v.y - ay * v.x) / horizontal;
      d.lateral += (lateral - d.lateral) * 0.15f;  // low pass: single frame spikes (hits) are the shake effect's job
    } else {
      d.lateral *= 0.9f;
    }
  } else if (!d.valid || dt >= 0.1f) {
    d.lateral = 0.0f;
  }
  d.vx    = v.x;
  d.vy    = v.y;
  d.stamp = now.QuadPart;
  d.tick  = GetTickCount();
  d.valid = true;
}

void __fastcall FF_TickDetour(void* self, void*, float steer, int joy) {
  g_Calls.ticks++;
  UpdateDriving(self, joy);
  if (joy == 0 && g_NosField.Known()) {
    uintptr_t vehicle = 0;
    float     level;
    if (SafeRead(self, vehicle) && vehicle && ReadNosLevel(vehicle, level)) {
      g_NosLevel = level;
      g_NosFull  = std::max(g_NosFull, level);
    }
  }
  if (gRumble.debugLog) LogForceTick(self, steer, joy);
  ((ForceTick_t)FF_TICK_ADDR)(self, steer, joy);
}

// pad motors don't turn at all below roughly 15% power: anything noticeable is lifted over that dead zone, and levels too small
// to feel are dropped instead of becoming a constant buzz
float OverDeadZone(float v) { return v < 0.04f ? 0.0f : 0.15f + 0.85f * std::min(v, 1.0f); }

float Live(DWORD tick, DWORD now, float value, float full) { return (now - tick < EFFECT_LIVE_MS) ? std::min(1.0f, std::max(0.0f, value / full)) : 0.0f; }

void Send(int slot, WORD low, WORD high, bool force) {
  const DWORD now     = GetTickCount();
  const bool  changed = std::abs((int)low - (int)g_LastLow[slot]) > 1300 || std::abs((int)high - (int)g_LastHigh[slot]) > 1300;  // ~2%
  if (!force && !changed && now - g_LastSend[slot] < SEND_INTERVAL_MS) return;
  g_LastLow[slot]  = low;
  g_LastHigh[slot] = high;
  g_LastSend[slot] = now;
  const bool ok    = GetGamepadBackend()->Rumble(slot, low, high, RUMBLE_DURATION_MS);

  static bool loggedFirst = false, loggedFail = false;
  if (!ok && !loggedFail) {
    loggedFail = true;
    Log("Rumble: the %s backend refused the rumble command (low=%u high=%u)", GetGamepadBackend()->Name(), (unsigned)low, (unsigned)high);
  }
  if (ok && (low || high) && !loggedFirst) {
    loggedFirst = true;
    Log("Rumble: first vibration sent through the %s backend (low=%u high=%u)", GetGamepadBackend()->Name(), (unsigned)low, (unsigned)high);
  }
}

}  // namespace

void InstallUndergroundForceFeedback() {
  // the code asks the input manager for everything, it has to exist (it normally does, this is for the case it is created later)
  g_ManagerStub = VirtualAlloc(NULL, 0x4000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

  injector::MakeJMP(FF_DEVICE_READY_ADDR, FF_DeviceReady, true);
  injector::MakeJMP(FF_HAS_CAPABILITY_ADDR, FF_HasCapability, true);
  injector::MakeJMP(FF_STOP_STEERING_ADDR, FF_StopSteering, true);
  injector::MakeJMP(FF_STOP_SECONDARY_ADDR, FF_StopSecondary, true);
  injector::MakeJMP(FF_IS_ACTIVE_ADDR, FF_IsEffectActive, true);
  injector::MakeJMP(FF_SPRING_ADDR, FF_Spring, true);
  injector::MakeJMP(FF_CONSTANT_ADDR, FF_ConstantForce, true);
  injector::MakeJMP(FF_DAMPER_ADDR, FF_Damper, true);
  injector::MakeJMP(FF_SHAKE_ADDR, FF_Shake, true);
  injector::MakeJMP(FF_OFFROAD_ADDR, FF_Offroad, true);
  injector::MakeJMP(FF_ROAD_ADDR, FF_Road, true);
  injector::MakeJMP(FF_STOP_ROAD_ADDR, FF_StopRoad, true);
  injector::MakeJMP(FF_NOTHING_ADDR1, FF_Nothing, true);
  injector::MakeJMP(FF_NOTHING_ADDR2, FF_Nothing, true);
  injector::MakeJMP(FF_ENGAGE_ADDR, FF_Engage, true);
  injector::MakeJMP(FF_DISENGAGE_ADDR, FF_Disengage, true);
  injector::MakeCALL(FF_TICK_CALL_ADDR1, FF_TickDetour, true);
  injector::MakeCALL(FF_TICK_CALL_ADDR2, FF_TickDetour, true);
  if (!gRumble.nosField.empty()) {
    unsigned a = 0, b = 0;
    const int n = sscanf(gRumble.nosField.c_str(), "%x:%x", &a, &b);
    if (n == 2) {
      g_NosField.pointerOffset = (int)a;
      g_NosField.offset        = (int)b;
    } else if (sscanf(gRumble.nosField.c_str(), "%x", &a) == 1) {
      g_NosField.offset = (int)a;
    }
    if (g_NosField.Known()) Log("Nitro level: read from NosField (pointer offset %d, offset 0x%X)", g_NosField.pointerOffset, g_NosField.offset);
  }
  Log("Rumble: the game's force feedback layer is replaced, pad rumble follows the game's own effects");
}

void UpdateRumble() {
  if (!*(void**)INPUTMANAGER_PTR_ADDR && g_ManagerStub) *(void**)INPUTMANAGER_PTR_ADDR = g_ManagerStub;

  const DWORD now = GetTickCount();
  for (int slot = 0; slot < 2; slot++) {
    if (!Usable(slot)) {
      if (g_LastLow[slot] || g_LastHigh[slot]) Send(slot, 0, 0, true);
      continue;
    }
    const ForceSlot& f = g_Force[slot];

    const float road    = Live(f.roadTick, now, f.road, ROAD_FULL);
    const float shake   = std::min(1.0f, std::max(0.0f, f.shake / EFFECT_MAX)) * (now - f.shakeTick < SHAKE_MS ? 1.0f - (float)(now - f.shakeTick) / (float)SHAKE_MS : 0.0f);
    const float offroad = Live(f.offroadTick, now, f.offroad, EFFECT_MAX);

    const DrivingState& d     = g_Drive[slot];
    const bool          fresh = d.valid && now - d.tick < 200;
    const float         speed = fresh ? d.speed : 0.0f;
    const float         speedKmh  = speed * 3.6f;
    const float         speedNorm = std::min(1.0f, std::max(0.0f, (speedKmh - gRumble.speedStart) / std::max(1.0f, gRumble.speedFull - gRumble.speedStart)));

    // nitro and handbrake are inputs: they only count while the car is moving
    // with NosField set the nitro only rumbles while there is nitrous left, otherwise it follows the button alone
    const bool  nosLeft = !g_NosField.Known() || (slot == 0 && g_NosLevel > 0.02f * g_NosFull && g_NosLevel > 1e-4f);
    const bool  nosHeld = nosLeft && (UndergroundEventHeld(slot, JOY_EVENT_NITRO_BOOST) || UndergroundEventHeld(slot, JOY_EVENT_NITRO_BOOST_ALTERNATE)) && speed > 3.0f;
    const bool  hbHeld  = (UndergroundEventHeld(slot, JOY_EVENT_EBRAKE) || UndergroundEventHeld(slot, JOY_EVENT_EBRAKE_ANALOG)) && speed > 8.0f;
    const float nos     = nosHeld ? 1.0f : 0.0f;
    const float brake   = hbHeld ? 0.4f + 0.6f * std::min(1.0f, speed / 30.0f) : 0.0f;
    const float drift   = (fresh && speed > 10.0f) ? std::min(1.0f, std::max(0.0f, (d.lateral - gRumble.driftStart) / std::max(0.1f, gRumble.driftFull - gRumble.driftStart))) : 0.0f;

    float low  = gRumble.road * road + gRumble.impact * 0.8f * shake + 0.5f * offroad + gRumble.speed * 0.6f * speedNorm + gRumble.nos * 0.6f * nos + gRumble.handbrake * 0.5f * brake +
                 gRumble.drift * 0.5f * drift;
    float high = gRumble.road * 0.5f * road + gRumble.impact * shake + 0.5f * offroad + gRumble.speed * 0.4f * speedNorm * speedNorm + gRumble.nos * nos + gRumble.handbrake * 0.6f * brake +
                 gRumble.drift * 0.7f * drift;
    low        = std::min(1.0f, low * gRumble.strength);
    high       = std::min(1.0f, high * gRumble.strength);
    Send(slot, (WORD)(OverDeadZone(low) * 65535.0f), (WORD)(OverDeadZone(high) * 65535.0f), false);

    if (gRumble.debugLog && now - g_LastDebug > 1000) {
      g_LastDebug = now;
      Log("Rumble: pad %d road=%.0f shake=%.0f offroad=%.0f speed=%.1f sideways=%.1f nos=%d (level %.3f of %.3f) handbrake=%d -> low=%.2f high=%.2f", slot, f.road, f.shake, f.offroad, speed, d.lateral, (int)nosHeld, g_NosLevel, g_NosFull, (int)hbHeld, low, high);
    }
  }
}

#else  // no car data for the other games yet

void UpdateRumble() {}

#endif
