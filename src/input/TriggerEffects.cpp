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
#include "input/TriggerEffects.h"
#include <sstream>

namespace {

// effect type ids from the DualSense firmware
enum : uint8_t {
  EFFECT_OFF       = 0x05,
  EFFECT_FEEDBACK  = 0x21,
  EFFECT_WEAPON    = 0x25,
  EFFECT_VIBRATION = 0x26,
};

// 10 zones, 3 bits each: strengths (1-8, stored as 0-7) packed into 30 bits plus a "zone is active" bit mask
void PackZones(const uint8_t strength[10], uint32_t& forceZones, uint16_t& activeZones) {
  forceZones  = 0;
  activeZones = 0;
  for (int i = 0; i < 10; i++) {
    if (strength[i] > 0) {
      forceZones |= (uint32_t)(((strength[i] - 1) & 0x07) << (3 * i));
      activeZones |= (uint16_t)(1 << i);
    }
  }
}

void WriteZones(uint8_t out[TRIGGER_EFFECT_SIZE], uint8_t type, uint16_t activeZones, uint32_t zones, uint8_t frequency) {
  out[0]  = type;
  out[1]  = (uint8_t)(activeZones & 0xFF);
  out[2]  = (uint8_t)((activeZones >> 8) & 0xFF);
  out[3]  = (uint8_t)(zones & 0xFF);
  out[4]  = (uint8_t)((zones >> 8) & 0xFF);
  out[5]  = (uint8_t)((zones >> 16) & 0xFF);
  out[6]  = (uint8_t)((zones >> 24) & 0xFF);
  out[7]  = 0;
  out[8]  = 0;
  out[9]  = frequency;
  out[10] = 0;
}

}  // namespace

bool TriggerEffectOff(uint8_t out[TRIGGER_EFFECT_SIZE]) {
  memset(out, 0, TRIGGER_EFFECT_SIZE);
  out[0] = EFFECT_OFF;
  return true;
}

bool TriggerEffectFeedback(uint8_t out[TRIGGER_EFFECT_SIZE], int position, int strength) {
  if (position < 0 || position > 9 || strength < 0 || strength > 8) return false;
  if (strength == 0) return TriggerEffectOff(out);

  uint8_t zones[10] = {};
  for (int i = position; i < 10; i++) zones[i] = (uint8_t)strength;
  uint32_t force;
  uint16_t active;
  PackZones(zones, force, active);
  WriteZones(out, EFFECT_FEEDBACK, active, force, 0);
  return true;
}

bool TriggerEffectWeapon(uint8_t out[TRIGGER_EFFECT_SIZE], int startPosition, int endPosition, int strength) {
  if (startPosition > 7 || startPosition < 2 || endPosition > 8 || endPosition <= startPosition || strength < 0 || strength > 8) return false;
  if (strength == 0) return TriggerEffectOff(out);

  const uint16_t startAndStopZones = (uint16_t)((1 << startPosition) | (1 << endPosition));
  memset(out, 0, TRIGGER_EFFECT_SIZE);
  out[0] = EFFECT_WEAPON;
  out[1] = (uint8_t)(startAndStopZones & 0xFF);
  out[2] = (uint8_t)((startAndStopZones >> 8) & 0xFF);
  out[3] = (uint8_t)(strength - 1);
  return true;
}

bool TriggerEffectVibration(uint8_t out[TRIGGER_EFFECT_SIZE], int position, int amplitude, int frequency) {
  if (position < 0 || position > 9 || amplitude < 0 || amplitude > 8 || frequency < 0 || frequency > 255) return false;
  if (amplitude == 0 || frequency == 0) return TriggerEffectOff(out);

  uint8_t zones[10] = {};
  for (int i = position; i < 10; i++) zones[i] = (uint8_t)amplitude;
  uint32_t amp;
  uint16_t active;
  PackZones(zones, amp, active);
  WriteZones(out, EFFECT_VIBRATION, active, amp, (uint8_t)frequency);
  return true;
}

bool TriggerEffectMultiPositionFeedback(uint8_t out[TRIGGER_EFFECT_SIZE], const uint8_t strength[10]) {
  uint32_t force;
  uint16_t active;
  PackZones(strength, force, active);
  if (!active) return TriggerEffectOff(out);
  WriteZones(out, EFFECT_FEEDBACK, active, force, 0);
  return true;
}

bool TriggerEffectMultiPositionVibration(uint8_t out[TRIGGER_EFFECT_SIZE], int frequency, const uint8_t amplitude[10]) {
  if (frequency < 0 || frequency > 255) return false;
  uint32_t amp;
  uint16_t active;
  PackZones(amplitude, amp, active);
  if (frequency == 0 || !active) return TriggerEffectOff(out);
  WriteZones(out, EFFECT_VIBRATION, active, amp, (uint8_t)frequency);
  return true;
}

bool TriggerEffectSlopeFeedback(uint8_t out[TRIGGER_EFFECT_SIZE], int startPosition, int endPosition, int startStrength, int endStrength) {
  if (startPosition < 0 || startPosition > 8 || endPosition > 9 || endPosition <= startPosition) return false;
  if (startStrength < 1 || startStrength > 8 || endStrength < 1 || endStrength > 8) return false;

  uint8_t     strength[10] = {};
  const float slope        = 1.0f * (float)(endStrength - startStrength) / (float)(endPosition - startPosition);
  for (int i = startPosition; i < 10; i++) {
    if (i <= endPosition)
      strength[i] = (uint8_t)std::nearbyintf(startStrength + slope * (float)(i - startPosition));  // C# Math.Round: halves to even
    else
      strength[i] = (uint8_t)endStrength;
  }
  return TriggerEffectMultiPositionFeedback(out, strength);
}

bool ParseTriggerEffect(const std::string& text, TriggerEffect& out) {
  // "Feedback 3 4", "weapon,2,6,5", anything after ';' is a comment
  std::string clean = text.substr(0, text.find(';'));
  for (char& c : clean)
    if (c == ',') c = ' ';
  std::istringstream in(clean);
  std::string        name;
  if (!(in >> name)) return false;
  std::transform(name.begin(), name.end(), name.begin(), ::tolower);

  TriggerEffect e;
  int           want = 0;
  if (name == "off" || name == "none") {
    e.kind = TriggerEffectKind::Off;
  } else if (name == "feedback") {
    e.kind = TriggerEffectKind::Feedback;
    want   = 2;
  } else if (name == "weapon") {
    e.kind = TriggerEffectKind::Weapon;
    want   = 3;
  } else if (name == "vibration") {
    e.kind = TriggerEffectKind::Vibration;
    want   = 3;
  } else if (name == "slope") {
    e.kind = TriggerEffectKind::Slope;
    want   = 4;
  } else {
    return false;
  }
  for (int i = 0; i < want; i++)
    if (!(in >> e.p[i])) return false;

  // reject parameters the hardware won't take, so a typo doesn't silently become "off"
  uint8_t probe[TRIGGER_EFFECT_SIZE];
  switch (e.kind) {
    case TriggerEffectKind::Feedback:
      if (!TriggerEffectFeedback(probe, e.p[0], e.p[1])) return false;
      break;
    case TriggerEffectKind::Weapon:
      if (!TriggerEffectWeapon(probe, e.p[0], e.p[1], e.p[2])) return false;
      break;
    case TriggerEffectKind::Vibration:
      if (!TriggerEffectVibration(probe, e.p[0], e.p[1], e.p[2])) return false;
      break;
    case TriggerEffectKind::Slope:
      if (!TriggerEffectSlopeFeedback(probe, e.p[0], e.p[1], e.p[2], e.p[3])) return false;
      break;
    default:
      break;
  }
  out = e;
  return true;
}

void BuildTriggerEffect(const TriggerEffect& e, uint8_t out[TRIGGER_EFFECT_SIZE]) {
  bool ok = false;
  switch (e.kind) {
    case TriggerEffectKind::Feedback:
      ok = TriggerEffectFeedback(out, e.p[0], e.p[1]);
      break;
    case TriggerEffectKind::Weapon:
      ok = TriggerEffectWeapon(out, e.p[0], e.p[1], e.p[2]);
      break;
    case TriggerEffectKind::Vibration:
      ok = TriggerEffectVibration(out, e.p[0], e.p[1], e.p[2]);
      break;
    case TriggerEffectKind::Slope:
      ok = TriggerEffectSlopeFeedback(out, e.p[0], e.p[1], e.p[2], e.p[3]);
      break;
    default:
      break;
  }
  if (!ok) TriggerEffectOff(out);
}
