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
#include <string>

// Rumble for the first pad while racing. The game has no pad vibration on PC, so it is derived from the player's car:
//   road/engine  a low rumble that grows with speed and throttle
//   NOS          a stronger rumble while the NOS button is held
//   impacts      a pulse when the car loses a large part of its speed within a split second
// Carbon only for now: the player and its speed are found through the game's own PlayerGetSpeed code, no fixed addresses.
struct RumbleConfig {
  bool  enable          = true;
  float strength        = 1.0f;  // overall scale, 0 turns everything off
#ifdef GAME_UG2
  float road            = 1.0f;  // scale of the road surface effect the game asks for
#else
  float road            = 0.15f; // road/engine level at full speed
#endif
  float nos             = 0.5f;
  float speed           = 0.15f; // Underground 2: slight rumble at high speed
  float speedStart      = 150.0f;  // km/h where it begins ...
  float speedFull       = 280.0f;  // ... and where it is at its level
  float drift           = 0.5f;  // Underground 2: rumble while sliding sideways
  float driftStart      = 7.0f;  // sideways acceleration (m/s^2) where it begins ...
  float driftFull       = 13.0f; // ... and where it is at its level
  std::string nosField;          // Underground 2: where the car keeps its nitrous ("0x3C4" or "0x40:0x3C4"), empty = learn it while the nitro is used
  float handbrake       = 0.6f;  // Underground 2: rumble while the handbrake is held at speed
  float impact          = 1.0f;
  bool  onlyWhileRacing = true;
  bool  debugLog        = false; // log speed and impact detection once a second
};

extern RumbleConfig gRumble;

// once per frame, after the pad state has been read
void UpdateRumble();

#ifdef GAME_UG2
// replaces the game's force feedback effect layer so its own wheel effects drive the pad
void InstallUndergroundForceFeedback();
#endif
