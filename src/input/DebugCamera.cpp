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
#include "input/DebugCamera.h"
#include "plugin/Plugin.h"
#include "input/InputDevice.h"

// mouselook vars (also read by Controller.cpp and Config.cpp, so they stay outside the Underground gate)
int   MouseLook_XSpeed     = 0;
int   MouseLook_YSpeed     = 0;
float MouseLookSensitivity = 1.0f;

#ifndef GAME_UNDERGROUND

#pragma runtime_checks("", off)

void(__thiscall* DebugWorldCameraMover_Update)(void* thethis, float unk)             = (void(__thiscall*)(void*, float))DebugWorldCameraMover_Update_Addr;

void*(__thiscall* DebugWorldCameraMover_Constructor)(void* thethis, void* vector1, void* vector2, int joyport,
                                                     int unk)      = (void*(__thiscall*)(void*, void*, void*, int, int))DEBUGWORLDCAMERAMOVER_CONSTRUCTOR_ADDR;

void*(__thiscall* DebugWorldCameraMover_Destructor)(void* thethis) = (void*(__thiscall*)(void*))DEBUGWORLDCAMERAMOVER_DESTRUCTOR_ADDR;

float DebugWorldCameraMover_16bit_max  = 65536.0f;
bool  bDisableDebugWorldCameraUpdating = true;
int DebugWorldCameraMover_Cave1_Exit     = DEBUGWORLDCAMERAMOVER_CAVE1_EXIT;
int DebugWorldCameraMover_Cave1_FullExit = DEBUGWORLDCAMERAMOVER_CAVE1_FULLEXIT;

void __declspec(naked) DebugWorldCameraMover_Cave1() {
  // clang-format off
  if (bDisableDebugWorldCameraUpdating) _asm jmp DebugWorldCameraMover_Cave1_FullExit;
#ifndef DEBUGWORLDCAMERAMOVER_MMX
  _asm {
      mov eax, MouseLook_XSpeed
#ifdef DEBUGWORLDCAMERAMOVER_FSTP
      fstp st
#endif
      mov[esp + DEBUGWORLDCAMERAMOVER_STACK_OFFSET], eax
      jmp DebugWorldCameraMover_Cave1_Exit
  }
#else  // NFS Undercover and World use the MMX registers and SSE2 instructions extensively
#ifdef GAME_UC
  _asm {
      movss xmm2, dword ptr[esp + 0x28]
      cvtps2pd xmm0, xmm0
      mov eax, MouseLook_XSpeed
      cvtsi2ss xmm1, eax
      jmp DebugWorldCameraMover_Cave1_Exit
  }
#endif
#ifdef GAME_WORLD
  _asm {
    movss xmm1, DebugWorldCameraMover_16bit_max
    movss xmm0, dword ptr[ebp - 0x18]
    mulss xmm0, xmm1 
    cvttss2si eax, xmm0
    movss xmm0, dword ptr[ebp - 0x20]
    movaps xmm2, xmm0
    mulss xmm2, xmm0
    movss xmm0, dword ptr[ebp - 0x1C]
    movaps xmm3, xmm0 
    mulss xmm3, xmm0
    addss xmm2, xmm3
    mov ecx, MouseLook_XSpeed
    add edi, ecx
    jmp DebugWorldCameraMover_Cave1_Exit
  }
#endif
#endif
  // clang-format on
}

int DebugWorldCameraMover_Cave2_Exit = DEBUGWORLDCAMERAMOVER_CAVE2_EXIT;

void __declspec(naked) DebugWorldCameraMover_Cave2() {
#ifndef DEBUGWORLDCAMERAMOVER_MMX
  // clang-format off
  _asm
  {
        mov eax, MouseLook_YSpeed
        mov	[esp+DEBUGWORLDCAMERAMOVER_STACK_OFFSET2], eax
        jmp DebugWorldCameraMover_Cave2_Exit
  }
#else  // NFS Undercover and World use the MMX registers and SSE2 instructions extensively
#ifdef GAME_UC
  _asm
  {
        mov edx, MouseLook_YSpeed
        movzx ecx, ax
        cvtsi2ss xmm0, edx
        jmp DebugWorldCameraMover_Cave2_Exit
  }
#endif
#ifdef GAME_WORLD
  _asm { 
    add eax, MouseLook_YSpeed
    mov[ebp - 4], eax
    jmp DebugWorldCameraMover_Cave2_Exit 
  }
#endif
#endif
  // clang-format on
}

// during DebugWorldCameraMover - set the cursor position to the center of the window for relative calculations
void __stdcall DebugWorldCameraMover_Update_Hook(float unk) {
  unsigned int thethis;
  _asm mov     thethis, ecx;

  // add original speeds
  MouseLook_XSpeed += *(short*)(thethis + DEBUGWORLDCAMERAMOVER_XSPEED_OFFSET);
  MouseLook_YSpeed += *(short*)(thethis + DEBUGWORLDCAMERAMOVER_YSPEED_OFFSET);

#ifndef DEBUGWORLDCAMERAMOVER_MMX
  if (!MouseLook_XSpeed && !MouseLook_YSpeed && !CurrValues[0][DEBUGACTION_MOVE_UP] && !CurrValues[0][DEBUGACTION_MOVE_DOWN])
    bDisableDebugWorldCameraUpdating = true;
  else
    bDisableDebugWorldCameraUpdating = false;
#else
  if (!MouseLook_XSpeed && !MouseLook_YSpeed && !CurrValues[0][DEBUGACTION_MOVE_UP] && !CurrValues[0][DEBUGACTION_MOVE_DOWN] &&
      !CurrValues[0][DEBUGACTION_MOVE_LEFT] && !CurrValues[0][DEBUGACTION_MOVE_RIGHT])
    bDisableDebugWorldCameraUpdating = true;
  else
    bDisableDebugWorldCameraUpdating = false;
#endif
  return DebugWorldCameraMover_Update((void*)thethis, unk);
}

void* __stdcall DebugWorldCameraMover_Constructor_Hook(void* vector1, void* vector2, int joyport, int unk) {
  unsigned int thethis;
  _asm mov     thethis, ecx;
  bInDebugWorldCamera = true;
  RECT windowRect;
  GetWindowRect(*(HWND*)GAME_HWND_ADDR, &windowRect);
  int CenterX = ((windowRect.right - windowRect.left) / 2) + windowRect.left;
  int CenterY = ((windowRect.bottom - windowRect.top) / 2) + windowRect.top;
  SetCursorPos(CenterX, CenterY);

  return DebugWorldCameraMover_Constructor((void*)thethis, vector1, vector2, joyport, unk);
}

void* DebugWorldCameraMover_Destructor_Hook() {
  unsigned int thethis;
  _asm mov     thethis, ecx;

  bInDebugWorldCamera = false;

  return DebugWorldCameraMover_Destructor((void*)thethis);
}

#pragma runtime_checks("", restore)

#endif  // !GAME_UNDERGROUND
