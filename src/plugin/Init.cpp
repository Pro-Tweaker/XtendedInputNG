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
#include "plugin/Init.h"
#include <injector/injector.hpp>
#include "plugin/Plugin.h"
#include "plugin/Config.h"
#include "plugin/Util.h"
#include "plugin/Log.h"
#include "input/GamepadBackend.h"
#include "input/ChaseCamera.h"
#include "game/ActionID.h"
#include "input/Controller.h"
#include "input/Mapping.h"
#include "input/InputDevice.h"
#include "input/InputMapping.h"
#include "input/DebugCamera.h"
#include "input/Rumble.h"
#include "fe/FEHooks.h"
#include "fe/FEng.h"

#ifdef GAME_CARBON
float FEActivationFloat = 0.999999f;

#endif
#ifdef GAME_PROSTREET
float FEActivationFloat = 0.999999f;

#endif
int Init() {
  FixWorkingDirectory();
  LogOpen();
  LogVirtualizationWarning();
  InitConfig();
  Log("Config loaded: backend=%s omni=%d radialDeadzone=%d mouselook=%d", gGamepadBackendName.c_str(), (int)bXInputOmniMode, (int)bRadialDeadzone, (int)bMouseLook);
  Log("Deadzones: LS=%.2f RS=%.2f (stick range 0..1, anything below is ignored); stick-digital=%.2f shifting=%.2f trigger-digital=%.2f", INPUT_DEADZONE_LS / 32767.0,
      INPUT_DEADZONE_RS / 32767.0, FEUPDOWN_ANALOG_THRESHOLD / 32767.0, SHIFT_ANALOG_THRESHOLD / 32767.0, TRIGGER_ACTIVATION_THRESHOLD / 255.0);

#ifndef GAME_UNDERGROUND
  injector::WriteMemory<unsigned int>(INPUTDEVICE_FACTORY_INITIALIZER_ADDR, (unsigned int)&InputDeviceFactory, true);
  injector::MakeCALL(INPUTMAPPING_CONSTRUCTOR_CALL_ADDR, InputMapping_Constructor, true);
#ifndef GAME_MW
  // Lower hardcoded deadzone to 0.000001 - VERY IMPORTANT
  injector::WriteMemory<unsigned int>(DEADZONE_FLOAT_POINTER_ADDR, (unsigned int)SMALL_FLOAT_ADDR, true);
#endif

  // kill DInput initialization
  injector::MakeNOP(DINPUT_KILL_ADDR, 5, true);
#endif  // !GAME_UNDERGROUND

#ifndef NO_FENG
  // dereference the current WndProc from the game executable and write to the function pointer (to maximize compatibility)
  GameWndProcAddr = *(unsigned int*)WNDPROC_POINTER_ADDR;
  GameWndProc     = (LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))GameWndProcAddr;
  injector::WriteMemory<unsigned int>(WNDPROC_POINTER_ADDR, (unsigned int)&CustomWndProc, true);

  // Mouse stuff
  injector::MakeJMP(FEMOUSE_UPDATE_CALL_ADDR, UpdateFECursorPos, true);

  // kill mouse wheel updating from InputDevice vars
  injector::MakeNOP(FEMOUSE_WHEEL_UPDATE_ADDR, 5, true);

  // window class register hook to change the default mouse cursor
  injector::MakeNOP(REGISTERCLASSHOOK_CALL_ADDR, 6, true);
  injector::MakeCALL(REGISTERCLASSHOOK_CALL_ADDR, RegisterClassHook, true);

  if (bUseWin32Cursor) {
    // disable PC_CURSOR rendering
    injector::MakeJMP(FEMOUSE_RENDER_JMP_FROM, FEMOUSE_RENDER_JMP_TO, true);
    // disable Win32 cursor hiding
    injector::MakeJMP(WIN32_MOUSEHIDE_JMP_FROM, WIN32_MOUSEHIDE_JMP_TO, true);
#ifndef GAME_MW
    injector::MakeNOP(SHOWCURSOR_HOOK_ADDR1, 6, true);
    injector::MakeCALL(SHOWCURSOR_HOOK_ADDR1, DummyFuncStd, true);
    injector::MakeNOP(SHOWCURSOR_HOOK_ADDR2, 6, true);
    injector::MakeCALL(SHOWCURSOR_HOOK_ADDR2, DummyFuncStd, true);
#endif

    if (LastControlledDevice == LASTCONTROLLED_CONTROLLER) bConsoleFEng = true;

  } else
    SetCursor(NULL);

  // kill garage camera control with FE pad buttons (leave only the stick)
  injector::MakeJMP(FENG_SHOWCASECAM_JMP_FROM, FENG_SHOWCASECAM_JMP_TO, true);

  // FEngHud -- joy handler fix
  injector::MakeJMP(FENGHUD_JOYHANDLER_JMP_FROM, FENGHUD_JOYHANDLER_JMP_TO, true);

  // FE VISUAL STUFF

  // custom Win32 mouse cursor
  if (bUseWin32Cursor) {
    if (bUseCustomCursor)
      NFSCursor = LoadCursorFromFile(cursorSkinPath);
    else
      NFSCursor = LoadCursor(NULL, IDC_ARROW);
  }

  // FORCE CONSOLE OBJECTS TO BE RENDERABLE ON PC
  // injector::MakeJMP(FENG_HIDEPCOBJ_JMP_FROM, FENG_HIDEPCOBJ_JMP_TO, true); // FEngHidePCObjects
  injector::WriteMemory<unsigned int>(FENG_HIDEPCOBJ_VT_ADDR, (unsigned int)&FEObjectCallback_Function, true);

#ifdef GAME_MW

  // dereference the current function after initing to maximize compatibility
  MouseStateArrayOffsetUpdater_Address = *(unsigned int*)FE_MOUSEUPDATER_CALLBACK_VT_ADDR;
  MouseStateArrayOffsetUpdater         = (bool(__thiscall*)(void*, void*))MouseStateArrayOffsetUpdater_Address;
  // it looks decieving, it's not related to mouse, it's for accessing all FEObjects during FE updating
  injector::WriteMemory<unsigned int>(FE_MOUSEUPDATER_CALLBACK_VT_ADDR, (unsigned int)&MouseStateArrayOffsetUpdater_Callback_Hook, true);

  if (bUseDynamicFEngSwitching) {
    injector::MakeNOP(CFENG_RENDEROBJ_NOP_ADDR, CFENG_RENDEOBJ_NOP_AMOUNT, true);  // cFEng render object
    injector::MakeJMP(FENG_SETVISIBLE_ADDR, FEngSetVisible, true);
    injector::MakeNOP(0x0052AEC3, 2, true);
    injector::MakeNOP(0x0052F33C, 2, true);
    injector::MakeNOP(0x0052F3FA, 6, true);
    injector::MakeNOP(0x00562C60, 2, true);
  } else {
    // make MW PC behave exactly like console version here... hide all PC elements instead
    if (LastControlledDevice == LASTCONTROLLED_CONTROLLER) {
      injector::WriteMemory<unsigned char>(0x005A45B9, FE_CONTROL_FLAG_PC, true);
      injector::WriteMemory<unsigned char>(0x00514CCD, FE_CONTROL_FLAG_PC, true);
      injector::WriteMemory<unsigned char>(0x0052AEC2, FE_CONTROL_FLAG_PC, true);
      injector::WriteMemory<unsigned char>(0x0052F33B, FE_CONTROL_FLAG_PC, true);
      injector::WriteMemory<unsigned char>(0x0052F3F9, FE_CONTROL_FLAG_PC, true);
      injector::WriteMemory<unsigned char>(0x00562C5F, FE_CONTROL_FLAG_PC, true);
    }
  }

  // remove the Controls option entry - it crashes the game
  injector::MakeJMP(0x0052905D, 0x005290B4, true);

  // hook for the performance tuning screen to show the setting names
  injector::WriteMemory<unsigned int>(0x008A2A78, (unsigned int)&CustomTuningScreen_NotificationMessage_Hook, true);
#else
  // Press START button initial hook...
  if (bEnableSplashTakeover) injector::MakeCALL(PRESS_START_HOOK_ADDR, FEngSetLanguageHash_Hook, true);

  if (bUseDynamicFEngSwitching) {
    injector::MakeNOP(CFENG_RENDEROBJ_NOP_ADDR, CFENG_RENDEOBJ_NOP_AMOUNT, true);  // cFEng render object
    injector::MakeCALL(CFENG_SERVICE_CALL_ADDR, cFEng_Service_Hook, true);
    injector::MakeCALL(FENGINE_PROCESSPADSFORPACKAGE_CALL_ADDR, FEngine_ProcessPadsForPackage_Hook, true);
#ifdef GAME_PROSTREET
    injector::MakeNOP(CFENG_RENDEROBJ_NOP2_ADDR, CFENG_RENDEOBJ_NOP_AMOUNT, true);  // cFEng render object
#endif
  } else {
    if (LastControlledDevice == LASTCONTROLLED_CONTROLLER) {
      injector::WriteMemory<unsigned char>(CFENG_RENDEROBJ_FLAG_ADDR, FE_CONTROL_FLAG_PC, true);
    }
  }
  // force analog zooming in FE orbit camera
  injector::MakeJMP(FE_ANALOGZOOM_JMP_FROM, FE_ANALOGZOOM_JMP_TO, true);
  // remove deadzone for FE activations...
  injector::WriteMemory<int>(FE_DEADZONE_POINTER_ADDR, (int)&FEActivationFloat, true);

  injector::MakeJMP(FEPHOTOMODE_HANDLESCREENTICK_HOOK_ADDR, FEPhotoModeStateManager_HandleScreenTick_Hook, true);
  injector::WriteMemory<unsigned int>(FEPHOTOMODE_HANDLELTRIGGER_HOOK_ADDR, (unsigned int)&FEPhotoModeStateManager_HandleLTrigger_Hook, true);
  injector::WriteMemory<unsigned int>(FEPHOTOMODE_HANDLERTRIGGER_HOOK_ADDR, (unsigned int)&FEPhotoModeStateManager_HandleRTrigger_Hook, true);

#ifdef GAME_CARBON
  injector::MakeCALL(0x005C3916, FEPackage_FindObjectByHash_Show_Hook, true);
  injector::MakeCALL(0x005CDD6D, FEPackage_FindObjectByHash_Hide_Hook, true);
  injector::MakeCALL(0x005B9023, FEPackage_FindObjectByHash_Hide_Hook, true);
  injector::MakeCALL(0x005AF3EE, FEPackage_FindObjectByHash_Hide_Hook, true);

  injector::MakeCALL(0x005AED20, FEPrintf_Hook_WorldMap, true);

  injector::MakeNOP(0x008577AC, 2, true);

  injector::WriteMemory<unsigned int>(FEWORLDMAPSTATEMANAGER_TICK_VT_ADDR, (unsigned int)&FEWorldMapStateManager_HandleScreenTick_Hook, true);

  if (bUseDynamicFEngSwitching) {
    injector::MakeCALL(0x005C670E, WorldMap_UnfocusQuickList_Hook, true);
    injector::MakeCALL(0x005CE0B9, WorldMap_SetQuickListInFocus_Hook, true);
    injector::MakeCALL(0x005CE309, WorldMap_SetQuickListInFocus_Hook, true);

  } else {
    if (LastControlledDevice == LASTCONTROLLED_CONTROLLER) {
      injector::WriteMemory<unsigned int>(0x005C3910, WORLDMAP_BUTTONGROUP_CONSOLE, true);
      injector::WriteMemory<unsigned int>(0x005CDD67, WORLDMAP_BUTTONGROUP_CONSOLE, true);
      injector::WriteMemory<unsigned int>(0x005B901D, WORLDMAP_BUTTONGROUP_CONSOLE, true);
      injector::WriteMemory<unsigned int>(0x005AF3E8, WORLDMAP_BUTTONGROUP_CONSOLE, true);
    }
  }

#endif  // GAME_CARBON
#ifdef GAME_PROSTREET
  // fix the "view details" string update in the FEHubMap screen
  injector::MakeCALL(0x007ECD78, FE_SetLanguageHash_Hook, true);
  injector::MakeCALL(0x007ECD59, FE_SetLanguageHash_Hook, true);
  injector::MakeCALL(0x007ECD0D, FE_SetLanguageHash_Hook, true);
  // remapping
  injector::MakeCALL(0x007028AA, GetRemappingModeHook, true);
  injector::MakeCALL(0x006FC03B, GoRemapButtonHook, true);
  injector::MakeCALL(0x6FC0E9, GoClearButtonHook, true);
  injector::MakeJMP(0x006A5940, GetNumExternalDevices, true);
  // The virtual controller has no legacy DirectInput binding slots. Restoring its
  // native defaults would write past the profile's binding tables when no
  // DirectInput controllers were enumerated. XtendedInputNG restores its own mappings.
  injector::MakeJMP(0x006B6D70, DummyFuncStd, true);
  injector::MakeCALL(0x005352FF, GetNumExternalDevicesZero, true);
  injector::MakeCALL(0x005354FC, GetNumExternalDevicesZero, true);
  injector::MakeCALL(0x00535596, GetNumExternalDevicesZero, true);
  injector::MakeJMP(0x006A5950, GetXtendedInputNGDeviceName, true);
  injector::MakeCALL(0x0070BCEC, GetMappingStringHook, true);
  injector::MakeCALL(0x0070BD0A, GetMappingStringHook, true);
  injector::WriteMemory<uintptr_t>(0x00991AFC, (uintptr_t)&ResetMappingsToDefault, true);
  // sell car string
  injector::WriteMemory<uint32_t>(0x005C6216, 0x6DBAAF8C, true);
  injector::WriteMemory<uint32_t>(0x005D8723, 0x6DBAAF8C, true);
  injector::WriteMemory<uint32_t>(0x005DDAF1, 0x6DBAAF8C, true);
  injector::WriteMemory<uint32_t>(0x005E2B3C, 0x6DBAAF8C, true);
  injector::WriteMemory<uint32_t>(0x005EB296, 0x6DBAAF8C, true);
  injector::WriteMemory<uint32_t>(0x007EACC0, 0x6DBAAF8C, true);
  injector::WriteMemory<uint32_t>(0x007EAD0B, 0x6DBAAF8C, true);

#endif

#endif

  SetUnbindableButtonTextures();
#endif  // GAME_UC

#ifdef GAME_WORLD
  // RECALCULATE FUNCTION POINTERS -- they shift around
  UTL_Com_Object_IList_Constructor = (void*(__thiscall*)(void*, unsigned int))UTL_ILIST_CONSTRUCTOR_ADDR;
  // InitProfileSettings = (void(*)())(0x756960 + MainBase);
  Managed_Alloc = (void* (*)(int))MANAGED_ALLOCATOR_ADDR;

  loc_75E375 = 0x75E375 + MainBase;
  loc_75E363 = 0x75E363 + MainBase;
  loc_75E237 = 0x75E237 + MainBase;
  loc_75E225 = 0x75E225 + MainBase;
  loc_75E25F = 0x75E25F + MainBase;

  injector::MakeJMP(0x0075E358 + MainBase, UnscrewWorldDeviceScalars, true);
  injector::MakeJMP(0x0075E217 + MainBase, UnscrewWorldDeviceScalars2, true);
  injector::MakeJMP(0x0075E254 + MainBase, UnscrewWorldDeviceScalars3, true);
#endif
#ifndef GAME_UNDERGROUND
  if (bMouseLook) {
    // Mouselook hook
    // dereference the current function after initing to maximize compatibility
    DebugWorldCameraMover_Update_Addr = *(unsigned int*)DEBUGWORLDCAMERAMOVER_UPDATE_VT_ADDR;
    DebugWorldCameraMover_Update      = (void(__thiscall*)(void*, float))DebugWorldCameraMover_Update_Addr;
    injector::WriteMemory<unsigned int>(DEBUGWORLDCAMERAMOVER_UPDATE_VT_ADDR, (unsigned int)&DebugWorldCameraMover_Update_Hook, true);

    injector::MakeJMP(DEBUGWORLDCAMERAMOVER_CAVE1_ENTRY, DebugWorldCameraMover_Cave1, true);
    injector::MakeJMP(DEBUGWORLDCAMERAMOVER_CAVE2_ENTRY, DebugWorldCameraMover_Cave2, true);
#ifdef GAME_WORLD
    DebugWorldCameraMover_Cave1_Exit     = DEBUGWORLDCAMERAMOVER_CAVE1_EXIT;
    DebugWorldCameraMover_Cave1_FullExit = DEBUGWORLDCAMERAMOVER_CAVE1_FULLEXIT;
    DebugWorldCameraMover_Cave2_Exit     = DEBUGWORLDCAMERAMOVER_CAVE2_EXIT;
    DebugWorldCameraMover_Constructor    = (void*(__thiscall*)(void*, void*, void*, int, int))DEBUGWORLDCAMERAMOVER_CONSTRUCTOR_ADDR;
    DebugWorldCameraMover_Destructor     = (void*(__thiscall*)(void*))DEBUGWORLDCAMERAMOVER_DESTRUCTOR_ADDR;
#endif

    // implement DebugWorldCameraMover detection -- for cursor hiding
    injector::MakeCALL(DEBUGWORLDCAMERAMOVER_CONSTRUCTOR_HOOK_ADDR, DebugWorldCameraMover_Constructor_Hook, true);
    injector::MakeCALL(DEBUGWORLDCAMERAMOVER_DESTRUCTOR_HOOK_ADDR, DebugWorldCameraMover_Destructor_Hook, true);
  }

#endif  // !GAME_UNDERGROUND

#ifdef GAME_UNDERGROUND
#ifdef GAME_UG2
  // custom config finder (the game's own returns nothing for the events we add)
  injector::MakeJMP(FINDSCANNERCONFIG_ADDR, FindScannerConfig_Custom, true);
#else
  // the game reads its scanner table through a pointer
  InstallUndergroundScannerTable();

  // disable writing to the device counter
  injector::MakeNOP(DEVICE_COUNTER_NOP_ADDR1, 6, true);
  injector::MakeNOP(DEVICE_COUNTER_NOP_ADDR2, 6, true);
  injector::MakeNOP(DEVICE_COUNTER_NOP_ADDR3, 5, true);
  injector::MakeNOP(DEVICE_COUNTER_NOP_ADDR4, 5, true);
  injector::MakeNOP(DEVICE_COUNTER_NOP_ADDR5, 5, true);
#endif

  // custom steering handler
  injector::WriteMemory<unsigned int>(STEER_HANDLER_ADDR1, (unsigned int)&RealDriver_JoyHandler_Steer, true);
  injector::WriteMemory<unsigned int>(STEER_HANDLER_ADDR2, (unsigned int)&DummyFunc, true);

  // kill DInput8 joypad reading and event generation
  injector::MakeJMP(EVENTGEN_JMP_ADDR_ENTRY, EVENTGEN_JMP_ADDR_EXIT, true);
  // this kills DInput enumeration COMPLETELY, even the keyboard
  injector::MakeJMP(DINPUTENUM_JMP_ADDR_ENTRY, DINPUTENUM_JMP_ADDR_EXIT, true);
  // kill game input reading
  injector::MakeJMP(JOYBUFFER_JMP_ADDR_ENTRY, JOYBUFFER_JMP_ADDR_EXIT, true);

  // replace ActualReadJoystickData with ReadControllerData
  injector::MakeCALL(ACTUALREADJOYDATA_CALL_ADDR1, ReadControllerData, true);
#ifdef GAME_UG
  injector::MakeCALL(ACTUALREADJOYDATA_CALL_ADDR2, ReadControllerData, true);
  injector::MakeCALL(ACTUALREADJOYDATA_CALL_ADDR3, ReadControllerData, true);
#endif

  // keyboard/device init
  injector::MakeCALL(INITJOY_CALL_ADDR, InitCustomKBInput, true);

  SetupScannerConfig();
#ifdef GAME_UG2
  InstallUndergroundButtonHints();
  InstallUndergroundForceFeedback();
  InstallChaseCamera();
#endif
#endif  // GAME_UNDERGROUND

  // per-user profile configs
#if defined(GAME_CARBON) || defined(GAME_PROSTREET)
  OnLoadDoneFunc = *(uintptr_t*)ONLOADDONE_FUNC_VTABLE_ADDR;
  injector::WriteMemory<uintptr_t>(ONLOADDONE_FUNC_VTABLE_ADDR, (uintptr_t)&OnLoadDoneHook, true);

#ifdef GAME_CARBON
  injector::WriteMemory<uintptr_t>(ONLOADDONE_FUNC_VTABLE_ADDR2, (uintptr_t)&OnLoadDoneHook, true);
#endif
#endif

#ifdef GAME_MW
  MemCardLoadAddr = reinterpret_cast<uintptr_t>(injector::MakeCALL(0x00563F33, MemoryCard_Load_Hook).get_raw<void>());
  injector::MakeCALL(0x00563CF7, MemoryCard_Load_Hook);
#endif
#ifdef GAME_CARBON
  InstallChaseCamera();
#endif
  // Init state
  ZeroMemory(g_Controllers, sizeof(CONTROLLER_STATE) * MAX_CONTROLLERS);

  // AttachConsole(ATTACH_PARENT_PROCESS);
  // AllocConsole();
  //freopen("CON", "w", stdout);
  //freopen("CON", "w", stderr);

  return 0;
}
