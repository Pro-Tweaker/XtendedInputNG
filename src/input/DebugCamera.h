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

extern void(__thiscall* DebugWorldCameraMover_Update)(void* thethis, float unk);
extern void*(__thiscall* DebugWorldCameraMover_Constructor)(void* thethis, void* vector1, void* vector2, int joyport, int unk);
extern void*(__thiscall* DebugWorldCameraMover_Destructor)(void* thethis);

// mouselook vars
extern int MouseLook_XSpeed;
extern int MouseLook_YSpeed;
extern float MouseLookSensitivity;
extern float DebugWorldCameraMover_16bit_max;
extern bool bDisableDebugWorldCameraUpdating;
extern int DebugWorldCameraMover_Cave1_Exit;
extern int DebugWorldCameraMover_Cave1_FullExit;
void DebugWorldCameraMover_Cave1();
extern int DebugWorldCameraMover_Cave2_Exit;
void DebugWorldCameraMover_Cave2();

// during DebugWorldCameraMover - set the cursor position to the center of the window for relative calculations
void __stdcall DebugWorldCameraMover_Update_Hook(float unk);
void* __stdcall DebugWorldCameraMover_Constructor_Hook(void* vector1, void* vector2, int joyport, int unk);
void* DebugWorldCameraMover_Destructor_Hook();
