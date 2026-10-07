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
#include <cstdint>
#include <string>

// Encoders for the 11-byte DualSense adaptive trigger effect block. The bit layout is a C++ port of Nielk1's
// "Factories for all DualSense trigger effects" (MIT, https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db).
// Positions and zones are 0-9 along the trigger's travel, strengths/amplitudes 0-8. Every encoder returns false (and
// leaves 'out' untouched) for arguments the hardware doesn't accept.
constexpr int TRIGGER_EFFECT_SIZE = 11;

bool TriggerEffectOff(uint8_t out[TRIGGER_EFFECT_SIZE]);
bool TriggerEffectFeedback(uint8_t out[TRIGGER_EFFECT_SIZE], int position, int strength);
bool TriggerEffectWeapon(uint8_t out[TRIGGER_EFFECT_SIZE], int startPosition, int endPosition, int strength);
bool TriggerEffectVibration(uint8_t out[TRIGGER_EFFECT_SIZE], int position, int amplitude, int frequency);
bool TriggerEffectMultiPositionFeedback(uint8_t out[TRIGGER_EFFECT_SIZE], const uint8_t strength[10]);
bool TriggerEffectMultiPositionVibration(uint8_t out[TRIGGER_EFFECT_SIZE], int frequency, const uint8_t amplitude[10]);
bool TriggerEffectSlopeFeedback(uint8_t out[TRIGGER_EFFECT_SIZE], int startPosition, int endPosition, int startStrength, int endStrength);

// An effect as written in the INI, e.g. "Feedback 3 4" or "Weapon 2 6 5" ("Off" turns it off).
//   Feedback  <position 0-9> <strength 1-8>                     resistance from that point of the pull onwards
//   Weapon    <start 2-7> <end 3-8> <strength 1-8>              resistance with a click-and-release at the end
//   Vibration <position 0-9> <amplitude 1-8> <frequency 1-255>  pulsing from that point onwards
//   Slope     <start 0-8> <end 1-9> <start strength 1-8> <end strength 1-8>   resistance ramping between two points
enum class TriggerEffectKind { Off, Feedback, Weapon, Vibration, Slope };

struct TriggerEffect {
  TriggerEffectKind kind = TriggerEffectKind::Off;
  int               p[4] = {0, 0, 0, 0};
};

// false when the text isn't a valid effect (the caller keeps its previous value)
bool ParseTriggerEffect(const std::string& text, TriggerEffect& out);

// encodes an effect, falling back to Off for invalid parameters
void BuildTriggerEffect(const TriggerEffect& effect, uint8_t out[TRIGGER_EFFECT_SIZE]);
