/*
// clang-format off
//
//    Xtended Input NG - native gamepad support for the PC versions of Need for Speed games
//    Continues xan1242's NFS-XtendedInput and NFSU-XtendedInput
//
//    MIT License
//
//    Copyright (c) 2022–2023 Lovro Pleše
//    Copyright (c) 2023 Berkay Yiğit <mail@berkay.link>
//    Copyright (c) 2026 Pro-Tweaker
//
// clang-format on
*/

#include "stdafx.h"
#include "plugin/Config.h"
#define MINI_CASE_SENSITIVE
#include "plugin/Plugin.h"
#include "plugin/Log.h"
#include "input/GamepadBackend.h"
#include "input/AdaptiveTriggers.h"
#include "input/Rumble.h"
#include "input/ChaseCamera.h"
#include "input/Controller.h"
#include "input/DebugCamera.h"
#include "fe/FEng.h"
#include "fe/FEHooks.h"
#include <mINI/src/mini/ini.h>

// INI values are user-edited, a bad one must never throw out of DllMain
static bool ParseIniLong(mINI::INIStructure& ini, const char* section, const char* key, long& out) {
  if (!ini[section].has(key)) return false;
  const std::string value = ini[section].get(key);
  char*             end   = NULL;
  long              v     = strtol(value.c_str(), &end, 10);
  if (end == value.c_str()) return false;
  out = v;
  return true;
}

static bool ParseIniFloat(mINI::INIStructure& ini, const char* section, const char* key, float& out) {
  if (!ini[section].has(key)) return false;
  const std::string value = ini[section].get(key);
  char*             end   = NULL;
  float             v     = strtof(value.c_str(), &end);
  if (end == value.c_str() || !std::isfinite(v)) return false;
  out = v;
  return true;
}

static void ReadIniBool(mINI::INIStructure& ini, const char* section, const char* key, bool& target) {
  long v;
  if (ParseIniLong(ini, section, key, v)) target = v != 0;
}

static void ReadIniPercent(mINI::INIStructure& ini, const char* section, const char* key, SHORT& target) {
  float v;
  if (ParseIniFloat(ini, section, key, v)) target = PercentToAxis(clampf(v, 0.0f, 1.0f));
}

// string value with an optional trailing ; comment and surrounding blanks removed
static bool ReadIniString(mINI::INIStructure& ini, const char* section, const char* key, std::string& out) {
  if (!ini[section].has(key)) return false;
  std::string value = ini[section].get(key);
  value             = value.substr(0, value.find(';'));
  value.erase(value.find_last_not_of(" 	") + 1);
  value.erase(0, value.find_first_not_of(" 	"));
  out = value;
  return true;
}

void InitConfig() {
  //CIniReader inireader("");
  mINI::INIFile      inifile(cfgIniName);
  mINI::INIStructure ini;
  inifile.read(ini);

  constexpr const char* inputSection = "Input";
  if (ini.has(inputSection)) {
    long v;
    if (ParseIniLong(ini, inputSection, "KeyboardReadingMode", v)) KeyboardReadingMode = (unsigned int)v;
    std::string name;
    if (ReadIniString(ini, inputSection, "Backend", name) && !name.empty()) gGamepadBackendName = name;
    ReadIniString(ini, inputSection, "IgnoreDevices", gSDLIgnoreDevices);
    ReadIniBool(ini, inputSection, "XInputOmniMode", bXInputOmniMode);
    ReadIniBool(ini, inputSection, "PassConnStatus", bPassConnStatus);
    ReadIniBool(ini, inputSection, "MouseLook", bMouseLook);
    ParseIniFloat(ini, inputSection, "MouseLookSensitivity", MouseLookSensitivity);

#ifndef NO_FENG
    ReadIniBool(ini, inputSection, "ConfineMouse", bConfineMouse);
    ReadIniBool(ini, inputSection, "UseWin32Cursor", bUseWin32Cursor);
    ReadIniBool(ini, inputSection, "UseCustomCursor", bUseCustomCursor);
    ReadIniBool(ini, inputSection, "EnableMouseHiding", bEnableMouseHiding);
#endif
  }

  constexpr const char* dzSection = "Deadzone";
  if (ini.has(dzSection)) {
    ReadIniBool(ini, dzSection, "RadialDeadzone", bRadialDeadzone);
    ReadIniPercent(ini, dzSection, "PercentLS", INPUT_DEADZONE_LS);
    ReadIniPercent(ini, dzSection, "PercentRS", INPUT_DEADZONE_RS);
    ReadIniPercent(ini, dzSection, "PercentLS_P2", INPUT_DEADZONE_LS_P2);
    ReadIniPercent(ini, dzSection, "PercentRS_P2", INPUT_DEADZONE_RS_P2);
    ReadIniPercent(ini, dzSection, "Percent_Shifting", SHIFT_ANALOG_THRESHOLD);
    ReadIniPercent(ini, dzSection, "Percent_AnalogStickDigital", FEUPDOWN_ANALOG_THRESHOLD);
    float triggerPercent;
    if (ParseIniFloat(ini, dzSection, "Percent_AnalogTriggerDigital", triggerPercent)) TRIGGER_ACTIVATION_THRESHOLD = PercentToTrigger(clampf(triggerPercent, 0.0f, 1.0f));
  }

  constexpr const char* rumSection = "Rumble";
  if (ini.has(rumSection)) {
    ReadIniBool(ini, rumSection, "Enable", gRumble.enable);
    ReadIniBool(ini, rumSection, "OnlyWhileRacing", gRumble.onlyWhileRacing);
    ReadIniBool(ini, rumSection, "Debug", gRumble.debugLog);
    ParseIniFloat(ini, rumSection, "Strength", gRumble.strength);
    ParseIniFloat(ini, rumSection, "Road", gRumble.road);
    ParseIniFloat(ini, rumSection, "NOS", gRumble.nos);
    ParseIniFloat(ini, rumSection, "Impact", gRumble.impact);
    ParseIniFloat(ini, rumSection, "Speed", gRumble.speed);
    ParseIniFloat(ini, rumSection, "Drift", gRumble.drift);
    ParseIniFloat(ini, rumSection, "Handbrake", gRumble.handbrake);
    ParseIniFloat(ini, rumSection, "SpeedStart", gRumble.speedStart);
    ParseIniFloat(ini, rumSection, "SpeedFull", gRumble.speedFull);
    ParseIniFloat(ini, rumSection, "DriftStart", gRumble.driftStart);
    ParseIniFloat(ini, rumSection, "DriftFull", gRumble.driftFull);
    ReadIniString(ini, rumSection, "NosField", gRumble.nosField);
    gRumble.strength = clampf(gRumble.strength, 0.0f, 2.0f);
  }

  constexpr const char* trgSection = "Triggers";
  if (ini.has(trgSection)) {
    ReadIniBool(ini, trgSection, "Enable", gAdaptiveTriggers.enable);
    ReadIniBool(ini, trgSection, "OnlyWhileRacing", gAdaptiveTriggers.onlyWhileRacing);
    for (const char* key : {"LeftTrigger", "RightTrigger"}) {
      if (!ini[trgSection].has(key)) continue;
      TriggerEffect effect;
      const std::string text = ini[trgSection].get(key);
      if (ParseTriggerEffect(text, effect))
        (key[0] == 'L' ? gAdaptiveTriggers.left : gAdaptiveTriggers.right) = effect;
      else
        Log("Config: [Triggers] %s = '%s' is not a valid effect, keeping the default", key, text.c_str());
    }
  }

#if defined(GAME_CARBON) || defined(GAME_UG2)
  constexpr const char* camSection = "Camera";
  if (ini.has(camSection)) {
    ReadIniBool(ini, camSection, "Enable", gChaseCamera.enable);
    ParseIniFloat(ini, camSection, "StickLookSensitivity", gChaseCamera.stickSensitivity);
    ParseIniFloat(ini, camSection, "ReturnTimeout", gChaseCamera.returnTimeoutSeconds);
    ParseIniFloat(ini, camSection, "ReturnSpeed", gChaseCamera.returnSpeed);
    ReadIniBool(ini, camSection, "InvertLook", gChaseCamera.invertLook);
    ReadIniBool(ini, camSection, "OnlyAllowHorizontalStickMovement", gChaseCamera.onlyAllowHorizontalStickMovement);
    ReadIniBool(ini, camSection, "ReturnOnRelease", gChaseCamera.returnOnRelease);
    ReadIniBool(ini, camSection, "Debug", gChaseCamera.debugLog);
    ReadIniBool(ini, camSection, "MouseLook", gChaseCamera.mouseLook);
    ParseIniFloat(ini, camSection, "MouseLookSensitivity", gChaseCamera.mouseSensitivity);
    ParseIniFloat(ini, camSection, "StickCurve", gChaseCamera.stickCurve);
    ParseIniFloat(ini, camSection, "PitchMin", gChaseCamera.pitchMin);
    ParseIniFloat(ini, camSection, "PitchMax", gChaseCamera.pitchMax);
    ReadIniBool(ini, camSection, "RecenterOnStickClick", gChaseCamera.recenterOnStickClick);
    // keep the response curve and pitch limits sane
    gChaseCamera.stickCurve = std::max(0.2f, std::min(5.0f, gChaseCamera.stickCurve));
    if (gChaseCamera.pitchMin > gChaseCamera.pitchMax) std::swap(gChaseCamera.pitchMin, gChaseCamera.pitchMax);
  }
#endif

#ifdef GAME_UG2
  constexpr const char* ugIcoSection = "Icons";
  if (ini.has(ugIcoSection)) {
    long v;
    if (ParseIniLong(ini, ugIcoSection, "ControllerIconMode", v)) {
      bAutoControllerIcons = (v < 0);  // -1 = follow the connected pad
      if (v >= 0) ControllerIconMode = v;
    }
    if (ParseIniLong(ini, ugIcoSection, "FirstControlDevice", v)) LastControlledDevice = v;
    ReadIniBool(ini, ugIcoSection, "ShowButtonHints", bShowPadHints);
    ReadIniBool(ini, ugIcoSection, "ShowKeyboardHints", bShowKeyboardHints);
  }
#endif

#ifndef NO_FENG
  constexpr const char* icoSection = "Icons";
  if (ini.has(icoSection)) {
    long v;
    if (ParseIniLong(ini, icoSection, "ControllerIconMode", v)) {
      bAutoControllerIcons = (v < 0);  // -1 = follow the connected pad
      if (v >= 0) ControllerIconMode = v;
    }
    if (ParseIniLong(ini, icoSection, "FirstControlDevice", v)) LastControlledDevice = v;
    ReadIniBool(ini, icoSection, "UseDynamicFEngSwitching", bUseDynamicFEngSwitching);
    ReadIniBool(ini, icoSection, "EnableSplashTakeover", bEnableSplashTakeover);
    if (ini[icoSection].has("ButtonTexFilename")) {
      strncpy(ButtonTexFilename, ini[icoSection].get("ButtonTexFilename").c_str(), sizeof(ButtonTexFilename) - 1);
      ButtonTexFilename[sizeof(ButtonTexFilename) - 1] = 0;
    }
  }
#endif
}
