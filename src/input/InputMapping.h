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
#include "input/InputDevice.h"

#ifndef GAME_WORLD
extern void*(__thiscall* FastMem_Alloc)(void* FastMem, int size, char* debug_str);

#endif
#ifdef GAME_MW
extern void* (*FastMem_InitListAllocator)();
extern void*(__stdcall* FastMem_ListAllocator)(void* CurrentListPos, void* NextListPos, InputMapEntry* inInputMap);
extern void*(__thiscall* STL_List_Add)(void* thethis, int num);
void* __stdcall InputMapping_Constructor(InputDevice* device, void* AttribCollection);

#else
#ifndef GAME_WORLD
void* __stdcall FastMem_ListAllocator(void* CurrentListPos, void* NextListPos, InputMapEntry* inInputMap);

#else
extern void* (*Managed_Alloc)(int size);
void* __stdcall FastMem_ListAllocator(void* CurrentListPos, void* NextListPos, InputMapEntry* inInputMap);

#endif
void* __stdcall InputMapping_Constructor(InputDevice* device, void* AttribCollection);

#endif

#endif  // !GAME_UNDERGROUND
