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

#pragma once

#ifndef GAME_UNDERGROUND
#include <new>
#include "game/ActionID.h"

extern void*(__thiscall* UTL_Com_Object_IList_Constructor)(void* thethis, unsigned int unk);

// global var for the action ID values
extern float PrevValues[2][MAX_ACTIONID];
extern float CurrValues[2][MAX_ACTIONID];

enum DeviceScalarType {
  kJoyAxis       = 0,
  kAnalogButton  = 1,
  kDigitalButton = 2,
};

struct DeviceScalarInfo {
  const char*      name;
  DeviceScalarType type;
  int              system_index;
  int              space[10];  // space for multiple binds?
};

class DeviceScalar {
 public:
  DeviceScalarType fType;
  unsigned int     fName_CRC32;
  float*           fPrevValue;
  float*           fCurrentValue;
  DeviceScalar() {
    fType         = (DeviceScalarType)0;
    fName_CRC32   = 0;
    fPrevValue    = NULL;
    fCurrentValue = NULL;
  }
};

enum ShiftPotential {
  SHIFT_POTENTIAL_NONE    = 0,
  SHIFT_POTENTIAL_DOWN    = 1,
  SHIFT_POTENTIAL_UP      = 2,
  SHIFT_POTENTIAL_GOOD    = 3,
  SHIFT_POTENTIAL_PERFECT = 4,
  SHIFT_POTENTIAL_MISS    = 5,
};

enum InputUpdateType {
  kUpdate        = 0,
  kPress         = 1,
  kRelease       = 2,
  kAnalogPress   = 3,
  kAnalogRelease = 4,
  kCenterControl = 5,
};

struct InputMapEntry {
  InputUpdateType UpdateType;
  float           LowerDZ;
  float           UpperDZ;
  ActionID        Action;
  int             DeviceScalarIndex;
#ifndef GAME_MW
  int ComboDeviceScalarIndex;
#endif
  float PreviousValue;
  float CurrentValue;
};

#ifndef GAME_WORLD
bool bIsHudVisible();

#else
bool bIsHudVisible();

// world, for whatever reason, uses direct pointers to the class instead of accessing the device scalar array normally
// copied this code snippet from Carbon 0x00696166
extern unsigned int loc_75E375;
extern unsigned int loc_75E363;
void UnscrewWorldDeviceScalars();
extern unsigned int loc_75E237;
extern unsigned int loc_75E225;
void UnscrewWorldDeviceScalars2();
extern unsigned int loc_75E25F;
void UnscrewWorldDeviceScalars3();

#endif
bool bCheckSecondBind();

// based on MW -- may change across games
class InputDevice {
  long Padding1;
  long FeedbackHandle;
  long UnkHandle;
  long UnkHandle2;
  long Padding2;

 public:
  DeviceScalar* fDeviceScalar;
  float*        fPrevValues;
  float*        fCurrentValues;
  int           fDeviceIndex;
  float         fControllerCurve;
#ifdef GAME_PROSTREET
  bool     bRemappingMode;
  uint32_t RemapType;
  uint32_t RemapIndex;
  uint32_t RemapIsPrimary;
#endif

  InputDevice(int DeviceIndex);
  virtual void* dtor(bool something);
  virtual bool IsConnected();
  virtual bool IsWheel() { return false; }
  virtual void Initialize();
  virtual void PollDevice();
  virtual int  GetNumDeviceScalar() { return MAX_ACTIONID; }
  virtual void StartVibration() {
    // printf("Called InputDevice::StartVibration\n");
  }
  virtual void StopVibration() {
    // this seems to be only called when the controller is unplugged...
    // printf("Called InputDevice::StopVibration\n");
  }
  virtual int GetInterfaces() { return 0; }
  virtual int GetSecondaryDevice() { return 0; }
#ifndef GAME_MW
  virtual int  GetBatteryLife() { return 0; }
  virtual bool IsBatteryLow() { return false; }
  virtual int  GetControllerTypeMask() { return 0; }
#endif
  virtual bool DeviceHasChanged() { return false; }
#ifndef GAME_MW
  virtual bool DeviceHasAnyActivity() { return false; }
#endif
  virtual void* SaveCurrentState() { return NULL; }
  virtual void  RestoreToState(float* unk) { return; }
#ifndef GAME_MW
  virtual int GetDeviceNoiseAllowance() { return 0; }
#endif
#ifdef GAME_PROSTREET
  virtual void GoRemapButton(uint32_t type, uint32_t button, uint32_t isPrimary, uint32_t unk);
#endif
};

InputDevice* InputDeviceFactory(int DeviceIndex);

#ifdef GAME_PROSTREET
void __stdcall GoRemapButtonHook(uint32_t type, uint32_t button_index, uint32_t isPrimary, uint32_t unk);
void __stdcall GoClearButtonHook(uint32_t type, uint32_t button_index, uint32_t isPrimary, uint32_t unk);
bool __stdcall GetRemappingModeHook();
void GetMappingStringHook(char* str, uint32_t type, uint32_t button_index, uint32_t isPrimary, uint32_t unk);
uint32_t GetNumExternalDevices();
uint32_t GetNumExternalDevicesZero();
const char* __stdcall GetXtendedInputNGDeviceName(uint32_t type, uint32_t index);

#endif

#endif  // !GAME_UNDERGROUND

#ifdef GAME_UNDERGROUND
struct ScannerConfig;
ScannerConfig* FindScannerConfig_Custom(unsigned int inJoyEvent, int confignum, int unk);
void           RealDriver_JoyHandler_Steer(int JoystickPort, int JoyEvent, char Value, void* RealDriver);
void           DummyFunc();
void           InitCustomKBInput();
void           SetupScannerConfig();
void           UpdateUndergroundDevices();
unsigned int   UndergroundPadBinding(unsigned int joyEvent);
unsigned int   UndergroundKeyBinding(unsigned int joyEvent);
bool           UndergroundEventHeld(int joy, unsigned int joyEvent);
void           UndergroundRightStickInUse(bool& horizontal, bool& vertical, bool& click);
#ifdef GAME_UG
void           InstallUndergroundScannerTable();
#endif
#endif
