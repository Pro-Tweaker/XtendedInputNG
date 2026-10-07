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
#include "input/AdaptiveTriggers.h"
#include "input/Controller.h"
#include "input/GamepadBackend.h"

AdaptiveTriggerConfig gAdaptiveTriggers;

namespace {

constexpr DWORD RESEND_MS = 2000;  // effects stay on the pad, this only recovers from a replug or a missed write

struct SlotState {
  uint8_t sentLeft[TRIGGER_EFFECT_SIZE]  = {};
  uint8_t sentRight[TRIGGER_EFFECT_SIZE] = {};
  DWORD   lastSend                       = 0;
  bool    everSent                       = false;
};
SlotState g_Slots[MAX_CONTROLLERS];

}  // namespace

void UpdateAdaptiveTriggers() {
  if (!gAdaptiveTriggers.enable) return;

  const bool racing = *(int*)GAMEFLOWMANAGER_STATUS_ADDR == 6;
  const bool active = racing || !gAdaptiveTriggers.onlyWhileRacing;

  uint8_t left[TRIGGER_EFFECT_SIZE], right[TRIGGER_EFFECT_SIZE];
  if (active) {
    BuildTriggerEffect(gAdaptiveTriggers.left, left);
    BuildTriggerEffect(gAdaptiveTriggers.right, right);
  } else {
    TriggerEffectOff(left);
    TriggerEffectOff(right);
  }

  const DWORD now = GetTickCount();
  for (int i = 0; i < MAX_CONTROLLERS; i++) {
    if (!g_Controllers[i].bConnected) {
      g_Slots[i].everSent = false;  // a pad that comes back needs the effect again
      continue;
    }
    SlotState& s = g_Slots[i];
    const bool changed = !s.everSent || memcmp(s.sentLeft, left, sizeof(left)) || memcmp(s.sentRight, right, sizeof(right));
    if (!changed && now - s.lastSend < RESEND_MS) continue;

    // nothing to turn off on a pad that never had an effect
    if (!s.everSent && !active) {
      s.everSent = true;
      memcpy(s.sentLeft, left, sizeof(left));
      memcpy(s.sentRight, right, sizeof(right));
      s.lastSend = now;
      continue;
    }

    GetGamepadBackend()->SetTriggerEffects(i, left, right);  // false on pads without adaptive triggers, retried every RESEND_MS
    memcpy(s.sentLeft, left, sizeof(left));
    memcpy(s.sentRight, right, sizeof(right));
    s.lastSend = now;
    s.everSent = true;
  }
}
