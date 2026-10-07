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
#include "input/ChaseCamera.h"

#if defined(GAME_CARBON) || defined(GAME_UG2)

#include <injector/injector.hpp>
#include "game/ActionID.h"
#include "input/Controller.h"
#include "input/InputDevice.h"
#include "input/Mapping.h"
#include "input/XInputConfig.h"
#include "plugin/Log.h"
#include "plugin/PatternScan.h"
#include "plugin/Plugin.h"

ChaseCameraConfig gChaseCamera;

namespace {

struct Vec3 {
  float x, y, z;
};
struct Mat4 {
  float m[16];
};

// ---------------------------------------------------------------- camera state
float StickLookSensitivity;
float YawOffset   = 0.0f;  // degrees from the game's own camera
float PitchOffset = 0.0f;
float IdleTimer   = 0.0f;
Vec3  PrevCarPos  = {0, 0, 0};
bool  PrevCarPosValid = false;
float CarSpeed2D      = 0.0f;
constexpr float CarMoveThreshold = 0.5f;
constexpr float DEG2RAD          = 0.0174533f;

volatile uint8_t  g_LookBehind = 0;  // written by the stub below
#ifdef GAME_UG2
void* g_CameraMover = NULL;  // the camera update's this, set by the thunk in front of LookAtHook
#endif
volatile uint32_t g_CameraMode = 0;
void*             g_Trampoline = NULL;

void (__cdecl* g_OriginalLookAt)(Mat4*, Vec3*, Vec3*, Vec3*) = NULL;

float LerpAngleDeg(float from, float to, float t) {
  float diff = to - from;
  while (diff > 180.0f) diff -= 360.0f;
  while (diff < -180.0f) diff += 360.0f;
  return from + diff * t;
}

// frame time, measured here instead of reading the game's own timer
float FrameSeconds() {
  static LARGE_INTEGER freq = {}, last = {};
  if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  float dt = last.QuadPart ? (float)((double)(now.QuadPart - last.QuadPart) / (double)freq.QuadPart) : 1.0f / 60.0f;
  last     = now;
  return std::min(dt, 0.1f);  // a hitch (loading, alt-tab) must not fling the camera
}

// which right-stick axes are already used by a driving action (e.g. up/down for shifting). Those axes can't also orbit
void RightStickAxesInUse(bool& horizontal, bool& vertical, bool& click) {
  horizontal = vertical = click = false;
#ifdef GAME_UG2
  UndergroundRightStickInUse(horizontal, vertical, click);
#else
  for (int i = 0; i < MAX_ACTIONID; i++) {
    ActionID a = (ActionID)i;
    if (bIsActionFrontEnd(a) || bIsActionDebug(a)) continue;
    for (WORD bind : {XInputBindings_PRIMARY[i], XInputBindings_SECONDARY[i]}) {
      switch (bind) {
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
        default:
          break;
      }
    }
  }
#endif
}

uint32_t LastCameraMode = 0;

// pixels the mouse moved since the last frame; the cursor is put back to the window centre every frame
void ReadMouseDelta(float& dx, float& dy) {
  dx = dy = 0.0f;
  HWND hwnd = *(HWND*)GAME_HWND_ADDR;
  if (!hwnd || GetForegroundWindow() != hwnd) return;

  RECT r;
  if (!GetWindowRect(hwnd, &r)) return;
  const int cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
  POINT     p;
  if (!GetCursorPos(&p)) return;

  static DWORD lastTick = 0;
  const DWORD  now      = GetTickCount();
  const bool   fresh    = (now - lastTick) > 250;  // first frame after a pause or menu: just recentre
  lastTick              = now;
  SetCursorPos(cx, cy);
  gChaseMouseLookTick = now;
  if (fresh) return;
  dx = (float)(p.x - cx);
  dy = (float)(p.y - cy);
}

#ifdef GAME_UG2
// UG2: the camera update's this holds the active camera mode and the look-behind flag
bool IsChaseMode(uint32_t mode) { return mode == 0 || mode == 1 || mode == 5; }
#else
bool IsChaseMode(uint32_t mode) { return mode == 2 || mode == 3 || mode == 4; }
#endif

void __cdecl LookAtHook(Mat4* out, Vec3* from, Vec3* to, Vec3* up) {
  const float dt = FrameSeconds();
#ifdef GAME_UG2
  if (g_CameraMover) {
    g_CameraMode = *(uint32_t*)((uint8_t*)g_CameraMover + 0x48);
    g_LookBehind = *(uint8_t*)((uint8_t*)g_CameraMover + 0x51);
  }
#endif

  // only the chase cameras (modes 2-4); hood/bumper/cockpit and looking behind stay untouched
  if (g_LookBehind || !IsChaseMode(g_CameraMode)) {
    if (gChaseCamera.debugLog) {
      static DWORD lastSkipLog = 0;
      DWORD        now         = GetTickCount();
      if (now - lastSkipLog > 1000) {
        lastSkipLog = now;
        Log("ChaseCamera: skipped, mode=%u lookBehind=%u (orbit needs mode 2, 3 or 4)", (unsigned)g_CameraMode, (unsigned)g_LookBehind);
      }
    }
    YawOffset = PitchOffset = IdleTimer = 0.0f;
    PrevCarPosValid                    = false;
    return g_OriginalLookAt(out, from, to, up);
  }

  if (PrevCarPosValid && dt > 0.0f) {
    float cdx = to->x - PrevCarPos.x, cdy = to->y - PrevCarPos.y;
    CarSpeed2D = sqrtf(cdx * cdx + cdy * cdy) / dt;
  }
  PrevCarPos      = *to;
  PrevCarPosValid = true;

  // right stick after our deadzone, -1..1 (stick right = +X, up = +Y)
  const XINPUT_GAMEPAD& pad = g_Controllers[0].state.Gamepad;
  float stickX              = -(float)pad.sThumbRX / 32767.0f;
  float stickY              = -(float)pad.sThumbRY / 32767.0f;
  bool  horizontalInUse = false, verticalInUse = false, clickInUse = false;
  RightStickAxesInUse(horizontalInUse, verticalInUse, clickInUse);
  if (horizontalInUse) stickX = 0.0f;
  if (verticalInUse || gChaseCamera.onlyAllowHorizontalStickMovement) stickY = 0.0f;

  // response curve (after the deadzone): finer control near the centre when > 1
  if (gChaseCamera.stickCurve != 1.0f) {
    stickX = copysignf(powf(fabsf(stickX), gChaseCamera.stickCurve), stickX);
    stickY = copysignf(powf(fabsf(stickY), gChaseCamera.stickCurve), stickY);
  }

  // a different chase camera (or coming back from another view) starts from the game's own position
  if (g_CameraMode != LastCameraMode) {
    LastCameraMode = g_CameraMode;
    YawOffset = PitchOffset = IdleTimer = 0.0f;
  }

  // right stick click recentres, unless that button is used by an action
  static bool clickWasDown = false;
  const bool  clickDown    = (pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
  if (gChaseCamera.recenterOnStickClick && !clickInUse && clickDown && !clickWasDown) YawOffset = PitchOffset = IdleTimer = 0.0f;
  clickWasDown = clickDown;

  float mouseDX = 0.0f, mouseDY = 0.0f;
  if (gChaseCamera.mouseLook) ReadMouseDelta(mouseDX, mouseDY);
  const float mouseScale = gChaseCamera.mouseSensitivity * 0.15f;  // degrees per pixel (already per frame, no time scaling)
  const float mouseYaw   = -mouseDX * mouseScale;
  const float mousePitch = mouseDY * mouseScale;

  if (gChaseCamera.debugLog) {
    static DWORD lastLog = 0;
    DWORD        now     = GetTickCount();
    if (now - lastLog > 1000) {
      lastLog = now;
      Log("ChaseCamera: mode=%u lookBehind=%u stick=(%.2f, %.2f) axesInUse(h=%d v=%d) offsets=(%.1f, %.1f)", (unsigned)g_CameraMode, (unsigned)g_LookBehind, stickX,
          stickY, (int)horizontalInUse, (int)verticalInUse, YawOffset, PitchOffset);
    }
  }

  const bool hasInput    = (fabsf(stickX) > 0.01f || fabsf(stickY) > 0.01f || mouseYaw != 0.0f || mousePitch != 0.0f);
  const bool carIsMoving = CarSpeed2D > CarMoveThreshold;

  if (hasInput) {
    const float frames = dt * 60.0f;  // sensitivity is per frame at 60 fps
    YawOffset += stickX * StickLookSensitivity * frames + mouseYaw;
    PitchOffset += (stickY * StickLookSensitivity * frames + mousePitch) * (gChaseCamera.invertLook ? -1.0f : 1.0f);
    IdleTimer = 0.0f;
  } else {
    IdleTimer += dt;
    const bool returnNow = gChaseCamera.returnOnRelease || (IdleTimer >= gChaseCamera.returnTimeoutSeconds && carIsMoving);
    if (returnNow) {
      float blend = 1.0f - expf(-gChaseCamera.returnSpeed * dt);
      YawOffset   = LerpAngleDeg(YawOffset, 0.0f, blend);
      PitchOffset = PitchOffset + (0.0f - PitchOffset) * blend;
      if (fabsf(YawOffset) < 0.3f && fabsf(PitchOffset) < 0.3f) YawOffset = PitchOffset = 0.0f;
    }
  }

  if (YawOffset == 0.0f && PitchOffset == 0.0f) return g_OriginalLookAt(out, from, to, up);

  PitchOffset = std::max(gChaseCamera.pitchMin, std::min(gChaseCamera.pitchMax, PitchOffset));

  // spherical coordinates of the game's camera around the car, then apply the offsets
  float dx = from->x - to->x, dy = from->y - to->y, dz = from->z - to->z;
  float dist   = sqrtf(dx * dx + dy * dy + dz * dz);
  float dist2D = sqrtf(dx * dx + dy * dy);
  float yaw    = atan2f(dy, dx) + YawOffset * DEG2RAD;
  float pitch  = atan2f(dz, dist2D) + PitchOffset * DEG2RAD;
  pitch        = std::max(-1.39626f, std::min(1.39626f, pitch));  // ~80 degrees, avoids gimbal lock

  from->x = to->x + dist * cosf(pitch) * cosf(yaw);
  from->y = to->y + dist * cosf(pitch) * sinf(yaw);
  from->z = to->z + dist * sinf(pitch);
  return g_OriginalLookAt(out, from, to, up);
}

#ifdef GAME_UG2
__declspec(naked) void LookAtThunk() {
  __asm {
    mov g_CameraMover, esi
    jmp LookAtHook
  }
}
#else
// Runs in place of "mov byte ptr [esi+disp32], imm8" inside the camera update: records which camera is active, then
// executes the displaced original instruction through the trampoline.
__declspec(naked) void CameraDataStub() {
  __asm {
    pushfd
    push eax
    movzx eax, byte ptr [esi + 0B1h]
    mov g_LookBehind, al
    mov eax, dword ptr [esi + 0ACh]
    mov g_CameraMode, eax
    pop eax
    popfd
    jmp dword ptr [g_Trampoline]
  }
}
#endif

}  // namespace

void InstallChaseCamera() {
  if (!gChaseCamera.enable) return;
  StickLookSensitivity = gChaseCamera.stickSensitivity;

#ifdef GAME_UG2
  // the look-at call of the chase camera update (the same place the Widescreen Fix patches for its own camera option)
  uint8_t* lookAtCall = FindPattern("E8 ? ? ? ? 8B 4E ? 8D 84 24 ? ? ? ? 50 51");
  if (!lookAtCall) {
    Log("ChaseCamera: game code not recognised (look-at call missing) - disabled");
    return;
  }
  g_OriginalLookAt = (void(__cdecl*)(Mat4*, Vec3*, Vec3*, Vec3*))injector::MakeCALL((uintptr_t)lookAtCall, (void*)&LookAtThunk, true).get_raw<void>();
  Log("ChaseCamera: enabled (look-at call at %p)", lookAtCall);
  return;
#else

  uint8_t* lookAtCall = FindPattern("E8 ? ? ? ? 8B 4E ? 8D 84 24 ? ? ? ? 50 51 E8");
  uint8_t* cameraData = FindPattern("C6 86 ? ? ? ? ? ? ? ? ? ? ? DF E0");
  if (!lookAtCall || !cameraData) {
    Log("ChaseCamera: game code not recognised (call %s, camera data %s) - disabled", lookAtCall ? "found" : "missing", cameraData ? "found" : "missing");
    return;
  }

  // trampoline: the 7-byte instruction we overwrite, then back to the code after it
  uint8_t* tramp = (uint8_t*)VirtualAlloc(NULL, 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if (!tramp) {
    Log("ChaseCamera: VirtualAlloc failed - disabled");
    return;
  }
  memcpy(tramp, cameraData, 7);
  tramp[7] = 0xE9;
  *(int32_t*)(tramp + 8) = (int32_t)((uint8_t*)(cameraData + 7) - (tramp + 12));
  g_Trampoline           = tramp;

  g_OriginalLookAt = (void(__cdecl*)(Mat4*, Vec3*, Vec3*, Vec3*))injector::MakeCALL((uintptr_t)lookAtCall, (void*)&LookAtHook, true).get_raw<void>();
  injector::MakeJMP((uintptr_t)cameraData, (void*)&CameraDataStub, true);
  injector::MakeNOP((uintptr_t)cameraData + 5, 2, true);
  Log("ChaseCamera: enabled (look-at call at %p, camera data at %p)", lookAtCall, cameraData);
#endif  // GAME_UG2
}

#endif  // GAME_CARBON || GAME_UG2
