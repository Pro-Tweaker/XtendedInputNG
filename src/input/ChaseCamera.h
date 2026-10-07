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

#if defined(GAME_CARBON) || defined(GAME_UG2)

// Right-stick orbit of the in-race chase camera (Carbon only), driven by our own deadzone and bindings.
// The approach (hooking the look-at matrix call of CubicCameraMover::Update and offsetting yaw/pitch around the car)
// follows ThirteenAG's NFSCarbon.WidescreenFix, whose own [CAMERA] option must be turned off (Enable = 0) when this is on.
struct ChaseCameraConfig {
  bool  enable                          = true;
  float stickSensitivity                = 1.0f;  // degrees per frame (at 60 fps) at full deflection
  float returnTimeoutSeconds            = 3.0f;  // idle time before the camera drifts back behind the car
  float returnSpeed                     = 2.0f;
  bool  invertLook                      = false;
  bool  onlyAllowHorizontalStickMovement = false;
  bool  returnOnRelease                 = false;  // swing straight back as soon as the stick is released
  bool  debugLog                        = false;  // once a second: camera mode and stick values
#ifdef GAME_UG2
  bool  mouseLook                       = false;   // mouse also orbits the camera (the cursor is kept at the window centre while driving)
#else
  bool  mouseLook                       = true;   // mouse also orbits the camera (the cursor is kept at the window centre while driving)
#endif
  float mouseSensitivity                = 1.0f;
  float stickCurve                      = 1.0f;   // 1 = linear, >1 = finer control near the centre
  float pitchMin                        = -30.0f; // degrees below / above the game's own camera height
  float pitchMax                        = 60.0f;
  bool  recenterOnStickClick            = true;   // pressing the right stick resets the camera (only if that button has no other binding)
};

extern ChaseCameraConfig gChaseCamera;

// installs the hooks when enabled; logs and does nothing if the game's code doesn't match
void InstallChaseCamera();

#endif  // GAME_CARBON || GAME_UG2
