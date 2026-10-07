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
#include "input/InputMapping.h"
#include "plugin/Plugin.h"
#include "input/Mapping.h"

#ifndef GAME_UNDERGROUND

#pragma runtime_checks("", off)

#ifndef GAME_WORLD
void*(__thiscall* FastMem_Alloc)(void* FastMem, int size, char* debug_str) = (void*(__thiscall*)(void*, int, char*))FASTMEM_ALLOC_ADDR;

#endif
#ifdef GAME_MW
void* (*FastMem_InitListAllocator)()                               = (void* (*)())FASTMEM_INITLISTALLOCATOR_ADDR;

void*(__stdcall* FastMem_ListAllocator)(void* CurrentListPos, void* NextListPos,
                                        InputMapEntry* inInputMap) = (void*(__stdcall*)(void*, void*, InputMapEntry*))FASTMEM_LISTALLOCATOR_ADDR;

void*(__thiscall* STL_List_Add)(void* thethis, int num)            = (void*(__thiscall*)(void*, int))STL_LIST_ADD_ADDR;

void* __stdcall InputMapping_Constructor(InputDevice* device, void* AttribCollection) {
  unsigned int thethis      = 0;
  unsigned int list_current = 0;
  unsigned int list_next    = 0;
  int          alloc_result = 0;

  _asm mov thethis, ecx;

  // use game's own memory management to avoid trouble
  *(void**)(thethis + 4) = (void*)FastMem_InitListAllocator();
  list_current           = (unsigned int)*(void**)(thethis + 4);

  for (unsigned int i = 0; i < MAX_ACTIONID; i++) {
    if (VKeyBindings_PRIMARY[i] != 0 || VKeyBindings_SECONDARY[i] != 0 || XInputBindings_PRIMARY[i] != 0 || XInputBindings_SECONDARY[i] != 0)  // is it mapped
    {
      InputMapEntry map;
      if (device->fDeviceScalar[i].fType == kDigitalButton)
        map.UpdateType = kPress;
      else
        map.UpdateType = kUpdate;
      map.LowerDZ           = 0.0;
      map.UpperDZ           = 0.0;
      map.Action            = (ActionID)i;
      map.DeviceScalarIndex = i;
      map.PreviousValue     = -1.0f;
      map.CurrentValue      = -1.0f;
      list_next             = *(int*)(list_current + 4);
      alloc_result          = (int)FastMem_ListAllocator((void*)list_current, (void*)list_next, &map);
      // STL_List_Add((void*)thethis, 1);
      *(int*)(list_current + 4)   = alloc_result;
      **(int**)(alloc_result + 4) = alloc_result;
    }
  }

  return (void*)thethis;
}

#else
#ifndef GAME_WORLD
void* __stdcall FastMem_ListAllocator(void* CurrentListPos, void* NextListPos, InputMapEntry* inInputMap) {
  void* result = FastMem_Alloc((void*)GLOBAL_FASTMEM_ADDR, INIT_LIST_ALLOC_SIZE, NULL);
  if (result) {
    *(void**)result = CurrentListPos;
    *(void**)((int)result + 4) = NextListPos;
    memcpy((void*)((int)result + 8), inInputMap, sizeof(InputMapEntry));
  }
  return result;
}

#else
void* (*Managed_Alloc)(int size) = (void* (*)(int))MANAGED_ALLOCATOR_ADDR;

void* __stdcall FastMem_ListAllocator(void* CurrentListPos, void* NextListPos, InputMapEntry* inInputMap) {
  void* result = Managed_Alloc(INIT_LIST_ALLOC_SIZE);
  if (result) {
    *(void**)result            = CurrentListPos;
    *(void**)((int)result + 4) = NextListPos;
    memcpy((void*)((int)result + 8), inInputMap, sizeof(InputMapEntry));
  }
  return result;
}

#endif
void* __stdcall InputMapping_Constructor(InputDevice* device, void* AttribCollection) {
  volatile unsigned int thethis = 0;
  volatile unsigned int list_current = 0;
  volatile unsigned int list_next = 0;
  volatile int alloc_result = 0;

  _asm mov thethis, ecx;

  // use game's own memory management to avoid trouble
  *(void**)(thethis) = (void*)thethis;
  *(void**)(thethis + 4) = (void*)thethis;
  *(int*)(thethis + 8) = 0;

  for (unsigned int i = 0; i < MAX_ACTIONID; i++) {
    if (VKeyBindings_PRIMARY[i] != 0 || VKeyBindings_SECONDARY[i] != 0 || XInputBindings_PRIMARY[i] != 0 || XInputBindings_SECONDARY[i] != 0)  // is it mapped
    {
      InputMapEntry map;
      if (device->fDeviceScalar[i].fType == kDigitalButton)
        map.UpdateType = kPress;
      else
        map.UpdateType = kUpdate;

      map.LowerDZ = 0.0;
      map.UpperDZ = 0.0;
      map.Action = (ActionID)i;
      map.DeviceScalarIndex = i;
      map.ComboDeviceScalarIndex = -1;
      map.PreviousValue = -1.0f;
      map.CurrentValue = -1.0f;
      list_current = *(int*)(thethis + 4);
      list_next = *(int*)(list_current + 4);
      alloc_result = (int)FastMem_ListAllocator((void*)thethis, (void*)list_next, &map);

      *(int*)alloc_result = thethis;
      *(int*)(alloc_result + 4) = *(int*)(thethis + 4);
      **(int**)(thethis + 4) = alloc_result;
      *(int*)(thethis + 8) += 1;
      *(int*)(thethis + 4) = alloc_result;
    }
  }

  return (void*)thethis;
}

#endif

#pragma runtime_checks("", restore)

#endif  // !GAME_UNDERGROUND
