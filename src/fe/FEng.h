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

// The front end (FEng) integration does not exist for UC and World (NO_FENG)
#ifndef NO_FENG
#include "plugin/Plugin.h"
#include "fe/Buttons.h"

#include "game/Definitions.h"

#include <windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

// Win32 mouse API stuff
extern RECT windowRect;
extern bool bMousePressedDown;
extern bool bMousePressedDownOldState;
extern bool bMouse2PressedDown;
extern bool bMouse2PressedDownOldState;
extern bool bMouse3PressedDown;
extern bool bMouse3PressedDownOldState;
extern int MouseWheelValue;
extern float fFEScale;
extern bool bConfineMouse;
extern int TimeSinceLastMouseMovement;
extern bool bUseWin32Cursor;
extern bool bUseCustomCursor;
extern bool bLastUsedVirtualMouse;
extern bool bUseDynamicFEngSwitching;
extern bool bEnableMouseHiding;
extern HCURSOR NFSCursor;
#define MOUSEHIDE_TIME 5000

// FE texture button re-mapping
extern unsigned int ButtonTexHashes[FE_TEX_HASH_COUNT];
extern bool bLoadedConsoleButtonTex;
extern char ButtonTexFilename[128];

enum FEObjType {
  FE_None         = 0,
  FE_Image        = 1,
  FE_String       = 2,
  FE_Model        = 3,
  FE_List         = 4,
  FE_Group        = 5,
  FE_CodeList     = 6,
  FE_Movie        = 7,
  FE_Effect       = 8,
  FE_ColoredImage = 9,
  FE_AnimImage    = 10,
  FE_SimpleImage  = 11,
  FE_MultiImage   = 12,
  FE_UserMin      = 256,
};

enum FEObject_Flags {
  FF_HideInEdit            = -2147483648,
  FF_AffectAllScripts      = 0x40000000,
  FF_PerspectiveProjection = 0x20000000,
  FF_IsButton              = 0x10000000,
  FF_ObjectLocked          = 0x8000000,
  FF_IgnoreButton          = 0x4000000,
  FF_Dirty                 = 0x2000000,
  FF_DirtyTransform        = 0x1000000,
  FF_DirtyColor            = 0x800000,
  FF_DirtyCode             = 0x400000,
  FF_CodeSuppliedResource  = 0x200000,
  FF_UsesLibraryObject     = 0x100000,
  FF_DontNavigate          = 0x80000,
  FF_SaveStaticTracks      = 0x40000,
  FF_MouseObject           = 0x20000,
};

struct FEObject {
  void*          pDestructorCallback;
  long           Padding[2];
  unsigned long  GUID;
  unsigned long  NameHash;
  char*          pName;
  FEObjType      Type;
  unsigned long  Flags;
  unsigned short RenderContext;
  unsigned short ResourceIndex;
  unsigned long  Handle;
  unsigned long  UserParam;
  unsigned char* pData;
  unsigned long  DataSize;
  int            Responses;
  int            Scripts;
  void*          pCurrentScript;
  void*          Cached;
};
#ifdef GAME_MW
#define FE_CONTROL_FLAG_PC 0x8
#else
#define FE_CONTROL_FLAG_PC 0x1008
#endif
#define FE_CONTROL_FLAG_CONSOLE         0x40
#define CFE_FLAG_WAS_VISIBLE_ORIGINALLY 0x01000000

extern int(__thiscall* _cFEng_FindPackage)(void* cFEng, char* pkg);
extern bool(__thiscall* _cFEng_IsPackageInControl)(void* cFEng, char* pkg);
extern FEObject* (*FEngFindObject)(char* pkg_name, unsigned int obj_hash);

#ifdef GAME_MW
extern void (*FEngSetLanguageHash)(void* FEObject, unsigned int langhash);
#endif
extern void* (*CreateResourceFile)(char* filename, int ResFileType, int unk1, int unk2, int unk3);
extern void(__thiscall* ResourceFile_BeginLoading)(void* ResourceFile, void* callback, void* unk);
extern void (*ServiceResourceLoading)();
void __stdcall LoadResourceFile(char* filename, int ResType, int unk1, void* unk2, void* unk3, int unk4, int unk5);
int cFEng_FindPackageWithControl();

#ifdef GAME_MW
extern unsigned int MouseStateArrayOffsetUpdater_Address;
extern bool(__thiscall* MouseStateArrayOffsetUpdater)(void* CB_Obj, void* FEObject);
// void(*FEngSetVisible)(void* FEObject) = (void(*)(void*))FENG_SETVISIBLE_ADDR;
extern void (*FEngSetInvisible)(void* FEObject);
extern void(__thiscall* CustomTuningScreen_NotificationMessage)(void* thethis, unsigned int unk1, void* FEObject, unsigned int unk2, unsigned int unk3);

// custom FEngSetVisible - necessary to stop elements from appearing we don't want dynamically (flickering buttons)
void FEngSetVisible(FEObject* obj);
#else
void FEngSetLanguageHash_Hook(char* pkg_name, int obj_hash, int lang_hash);
extern void (*FE_Object_SetVisibility)(void* FEObject, bool visibility);
extern void(__thiscall* FEPackage_ForAllObjects)(void* FEPackage, void* CallbackVT);
extern void(__thiscall* cFEng_Service)(void* cFEng, int unk1, int unk2);
extern char* (*GetLocalizedString)(unsigned int langhash);
extern void(__thiscall* SelectCarCameraMover_SetHRotateSpeed)(void* SelectCarCameraMover, float speed, bool activate);
extern void(__thiscall* SelectCarCameraMover_SetVRotateSpeed)(void* SelectCarCameraMover, float speed, bool activate);
extern void(__thiscall* SelectCarCameraMover_SetZoomSpeed)(void* SelectCarCameraMover, float speed, bool activate);
extern void(__thiscall* FeGarageMain_ZoomCameraView)(void* FEGarageMain, float speed, bool activate);
extern void(__thiscall* FEngine_ProcessPadsForPackage)(void* FEngine, void* FEPackage);
#ifdef GAME_CARBON
extern void*(__thiscall* FEPackage_FindObjectByHash)(void* FEPackage, unsigned int namehash);
extern void(__thiscall* FEWorldMapStateManager_HandleScreenTick)(void* FEWorldMapStateManager);
extern void(__thiscall* WorldMap_UnfocusQuickList)(void* WorldMap);
extern void(__thiscall* WorldMap_SetQuickListInFocus)(void* WorldMap);

#endif

void FEngSetVisible(void* FEObject);
void FEngSetInvisible(void* FEObject);

#endif

char* cFEng_FindPackageWithControl_Name();

// fast version using hashes instead of string comparison
bool cFEng_IsPackageInControl_Fast(unsigned int pkg_name_hash);
unsigned int __stdcall FEngSetTextureHash(unsigned int FEImage, int hash);
unsigned int __stdcall FEngGetTextureHash(unsigned int FEImage);
bool FEngSwapTextureHash(FEObject* inobj, unsigned int target_hash, unsigned int new_hash);
int __stdcall FEngFindImage(char* pkgname, int hash);

#ifndef GAME_PROSTREET
extern void (*FESendKeystroke)(unsigned int key);
#endif

// functions wrapped because the compiler loves to check the ESP during debugging which causes errors
int __stdcall cFEng_FindPackage(char* pkg);
bool __stdcall cFEng_IsPackageInControl(char* pkg);
void FEngSetVisible_Name(char* pkg_name, unsigned int obj_hash);
void FEngSetInvisible_Name(char* pkg_name, unsigned int obj_hash);
int FEngSetTextureHash_Name(char* pkg_name, unsigned int obj_hash, unsigned int tex_hash);
void FEngSetVisible_CheckObj(FEObject* inobj, unsigned int obj_hash);
void FEngSetInvisible_CheckObj(FEObject* inobj, unsigned int obj_hash);

#ifdef GAME_MW
void FEngSetLanguageHash_CheckObj(FEObject* inobj, unsigned int obj_hash, unsigned int lang_hash);
#endif

void* FEngSetTextureHash_CheckObj(FEObject* inobj, unsigned int obj_hash, unsigned int tex_hash);
void UpdateFECursorPos();
bool bIsHudVisible();

#ifdef GAME_MW
void __stdcall CustomTuningScreen_NotificationMessage_Hook(unsigned int unk1, void* FEObject, unsigned int unk2, unsigned int unk3);
#endif

void SetUnbindableButtonTextures();
void SetBindingButtonTexture(ActionID id, WORD buttonmask);
void SetControllerUnplugText();
void SetControllerFEng(FEObject* inobj);

#ifdef GAME_PROSTREET
bool bInPCControllerConfig();
#endif

void UpdateControllerFEng(FEObject* inobj);
bool __stdcall FEObjectCallback_Function(FEObject* inobj);

#ifdef GAME_MW
bool __stdcall MouseStateArrayOffsetUpdater_Callback_Hook(FEObject* inobj);

#else

struct FEObjectCallbackStruct {
  void* Destructor;
  void* Function;
};

void __stdcall cFEng_Service_Hook(int unk1, int unk2);
#ifdef GAME_CARBON
// world map fixes for Carbon
void __stdcall FEngine_ProcessPadsForPackage_Hook(void* FEPackage);
void* __stdcall FEPackage_FindObjectByHash_Hide_Hook(unsigned int namehash);
void* __stdcall FEPackage_FindObjectByHash_Show_Hook(unsigned int namehash);
extern bool bQuickListFocused;
void __stdcall FEWorldMapStateManager_HandleScreenTick_Hook();
void __stdcall WorldMap_SetQuickListInFocus_Hook();
void __stdcall WorldMap_UnfocusQuickList_Hook();
void FEPrintf_Hook_WorldMap(char* pkg_name, unsigned int object_hash, char* fmt, char* str);

#endif  // GAME_CARBON

#ifdef GAME_PROSTREET
void __stdcall FEngine_ProcessPadsForPackage_Hook(void* FEPackage);
void FE_SetLanguageHash_Hook(char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);

#endif  // GAME_PROSTREET

#endif  // GAME_MW

#endif  // NO_FENG
