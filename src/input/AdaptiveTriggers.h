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
#include "input/TriggerEffects.h"

// DualSense adaptive trigger effects for the two triggers, applied while racing and switched off in menus.
// Needs Backend = SDL; other pads and the XInput backend ignore it.
struct AdaptiveTriggerConfig {
  bool          enable          = false;
  bool          onlyWhileRacing = true;  // false: also in menus
  TriggerEffect left;                    // brake
  TriggerEffect right;                   // throttle
};

extern AdaptiveTriggerConfig gAdaptiveTriggers;

// once per frame, after the pad state has been read
void UpdateAdaptiveTriggers();
