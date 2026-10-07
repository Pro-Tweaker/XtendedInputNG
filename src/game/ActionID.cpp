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
#include "game/ActionID.h"

#ifndef GAME_UNDERGROUND

unsigned int sub_5CC090(char* a1, int a2, unsigned int a3) {
  unsigned int v4;   // eax
  unsigned int v5;   // ecx
  unsigned int v6;   // ebp
  int          v7;   // edi
  unsigned int v9;   // edx
  unsigned int v10;  // ecx
  unsigned int v11;  // esi
  int          v12;  // eax
  unsigned int v13;  // ecx
  unsigned int v14;  // esi
  int          v15;  // eax
  unsigned int v16;  // ecx
  unsigned int v17;  // esi
  unsigned int v18;  // esi
  int          v19;  // edi
  unsigned int v20;  // ecx
  unsigned int v21;  // esi
  int          v22;  // edi
  unsigned int v23;  // ecx
  unsigned int v24;  // esi
  int          v25;  // edi

  v4 = a3;
  v5 = 0x9E3779B9;
  v6 = a3;
  v7 = 0x9E3779B9;
  if (a3 >= 0xC) {
    v9 = a3 / 0xC;
    do {
      v10 = *((int*)a1 + 1) + v5;
      v11 = *((int*)a1 + 2) + a2;
      v12 = (v11 >> 13) ^ (v7 + *(int*)a1 - v11 - v10);
      v13 = (v12 << 8) ^ (v10 - v11 - v12);
      v14 = (v13 >> 13) ^ (v11 - v13 - v12);
      v15 = (v14 >> 12) ^ (v12 - v14 - v13);
      v16 = (v15 << 16) ^ (v13 - v14 - v15);
      v17 = (v16 >> 5) ^ (v14 - v16 - v15);
      v7  = (v17 >> 3) ^ (v15 - v17 - v16);
      v5  = (v7 << 10) ^ (v16 - v17 - v7);
      a2  = (v5 >> 15) ^ (v17 - v5 - v7);
      a1 += 12;
      v6 -= 12;
      --v9;
    } while (v9);
    v4 = a3;
  }
  v18 = v4 + a2;
  switch (v6) {
    case 11:
      v18 += a1[10] << 24;
    case 10:
      v18 += a1[9] << 16;
    case 9:
      v18 += a1[8] << 8;
    case 8:
      v5 += a1[7] << 24;
    case 7:
      v5 += a1[6] << 16;
    case 6:
      v5 += a1[5] << 8;
    case 5:
      v5 += a1[4];
    case 4:
      v7 += a1[3] << 24;
    case 3:
      v7 += a1[2] << 16;
    case 2:
      v7 += a1[1] << 8;
    case 1:
      v7 += *a1;
      break;
    default:
      break;
  }

  v19 = (v18 >> 13) ^ (v7 - v18 - v5);
  v20 = (v19 << 8) ^ (v5 - v18 - v19);
  v21 = (v20 >> 13) ^ (v18 - v20 - v19);
  v22 = (v21 >> 12) ^ (v19 - v21 - v20);
  v23 = (v22 << 16) ^ (v20 - v21 - v22);
  v24 = (v23 >> 5) ^ (v21 - v23 - v22);
  v25 = (v24 >> 3) ^ (v22 - v24 - v23);
  return (((v25 << 10) ^ (v23 - v24 - v25)) >> 15) ^ (v24 - ((v25 << 10) ^ (v23 - v24 - v25)) - v25);
}

unsigned int stringhash32(char* a1) {
  unsigned int result;  // eax

  if (a1 && *a1)
    result = sub_5CC090(a1, 0xABCDEF00, strlen(a1));
  else
    result = 0;
  return result;
}

#ifdef GAME_MW
char* ActionIDStr[139] = {"NULL_ACTION",
                          "GAMEACTION_STEERLEFT",
                          "GAMEACTION_STEERRIGHT",
                          "GAMEACTION_TURNLEFT",
                          "GAMEACTION_TURNRIGHT",
                          "GAMEACTION_GAS",
                          "GAMEACTION_BRAKE",
                          "GAMEACTION_HANDBRAKE",
                          "GAMEACTION_GAMEBREAKER",
                          "GAMEACTION_SHIFTUP",
                          "GAMEACTION_SHIFTDOWN",
                          "GAMEACTION_NOS",
                          "GAMEACTION_RESET",
                          "GAMEACTION_FORWARD",
                          "GAMEACTION_BACK",
                          "GAMEACTION_JUMP",
                          "GAMEACTION_DEBUGHUMAN1",
                          "GAMEACTION_DEBUGHUMAN2",
                          "GAMEACTION_DEBUGHUMAN3",
                          "GAMEACTION_DEBUGHUMAN4",
                          "CAMERAACTION_CHANGE",
                          "CAMERAACTION_DEBUG",
                          "CAMERAACTION_ENABLE_ICE",
                          "CAMERAACTION_LOOKBACK",
                          "CAMERAACTION_PULLBACK",
                          "FRONTENDACTION_UP",
                          "FRONTENDACTION_DOWN",
                          "FRONTENDACTION_LEFT",
                          "FRONTENDACTION_RIGHT",
                          "FRONTENDACTION_RUP",
                          "FRONTENDACTION_RDOWN",
                          "FRONTENDACTION_RLEFT",
                          "FRONTENDACTION_RRIGHT",
                          "FRONTENDACTION_ACCEPT",
                          "FRONTENDACTION_CANCEL",
                          "FRONTENDACTION_CANCEL_ALT",
                          "FRONTENDACTION_START",
                          "FRONTENDACTION_BUTTON0",
                          "FRONTENDACTION_BUTTON1",
                          "FRONTENDACTION_BUTTON2",
                          "FRONTENDACTION_BUTTON3",
                          "FRONTENDACTION_BUTTON4",
                          "FRONTENDACTION_BUTTON5",
                          "FRONTENDACTION_LTRIGGER",
                          "FRONTENDACTION_RTRIGGER",
                          "HUDACTION_PAUSEREQUEST",
                          "HUDACTION_ENGAGE_EVENT",
                          "HUDACTION_PAD_LEFT",
                          "HUDACTION_PAD_DOWN",
                          "HUDACTION_PAD_RIGHT",
                          "HUDACTION_SKIPNIS",
                          "HUDACTION_NEXTSONG",
                          "VOIPACTION_PUSHTOTALK",
                          "DEBUGACTION_MOVE_UP",
                          "DEBUGACTION_MOVE_DOWN",
                          "DEBUGACTION_MOVE_LEFT",
                          "DEBUGACTION_MOVE_RIGHT",
                          "DEBUGACTION_MOVE_FORWARD",
                          "DEBUGACTION_MOVE_BACK",
                          "DEBUGACTION_MOVE_D_LEFT",
                          "DEBUGACTION_MOVE_D_RIGHT",
                          "DEBUGACTION_MOVE_D_FORWARD",
                          "DEBUGACTION_MOVE_D_BACK",
                          "DEBUGACTION_LOOK_UP",
                          "DEBUGACTION_LOOK_DOWN",
                          "DEBUGACTION_LOOK_LEFT",
                          "DEBUGACTION_LOOK_RIGHT",
                          "DEBUGACTION_LOOK_D_UP",
                          "DEBUGACTION_LOOK_D_DOWN",
                          "DEBUGACTION_LOOK_D_LEFT",
                          "DEBUGACTION_LOOK_D_RIGHT",
                          "DEBUGACTION_TURBO",
                          "DEBUGACTION_SUPER_TURBO",
                          "DEBUGACTION_DROPCAR",
                          "DEBUGACTION_TOGGLEAI",
                          "DEBUGACTION_TOGGLESIMSTEP",
                          "DEBUGACTION_SIMSTEP",
                          "DEBUGACTION_SCREENSHOT",
                          "DEBUGACTION_STOPRECORDPATH",
                          "DEBUGACTION_TOGGLECARCOLOUR",
                          "DEBUGACTION_TOGGLEDEMOCAMERAS",
                          "ICEACTION_GRAB_LEFT",
                          "ICEACTION_GRAB_RIGHT",
                          "ICEACTION_LEFT",
                          "ICEACTION_RIGHT",
                          "ICEACTION_FAST_LEFT",
                          "ICEACTION_FAST_RIGHT",
                          "ICEACTION_UP",
                          "ICEACTION_DOWN",
                          "ICEACTION_HELP",
                          "ICEACTION_UNDO",
                          "ICEACTION_PLAY",
                          "ICEACTION_HIDE",
                          "ICEACTION_SCREENSHOT",
                          "ICEACTION_SCREENSHOT_STREAM",
                          "ICEACTION_INSERT",
                          "ICEACTION_CUT",
                          "ICEACTION_COPY",
                          "ICEACTION_PASTE",
                          "ICEACTION_SELECT",
                          "ICEACTION_CANCEL",
                          "ICEACTION_ALT_1",
                          "ICEACTION_CLIP_IN",
                          "ICEACTION_CLIP_OUT",
                          "ICEACTION_SIMSPEED_INC",
                          "ICEACTION_SIMSPEED_DEC",
                          "ICEACTION_DUTCH_LEFT",
                          "ICEACTION_DUTCH_RIGHT",
                          "ICEACTION_ZOOM_IN",
                          "ICEACTION_ZOOM_OUT",
                          "ICEACTION_LETTERBOX_IN",
                          "ICEACTION_LETTERBOX_OUT",
                          "ICEACTION_BUBBLE_IN",
                          "ICEACTION_BUBBLE_OUT",
                          "ICEACTION_BUBBLE_UP",
                          "ICEACTION_BUBBLE_DOWN",
                          "ICEACTION_BUBBLE_TILT_UP",
                          "ICEACTION_BUBBLE_TILT_DOWN",
                          "ICEACTION_BUBBLE_SPIN_LEFT",
                          "ICEACTION_BUBBLE_SPIN_RIGHT",
                          "ICEACTION_BUBBLE_LEFT",
                          "ICEACTION_BUBBLE_RIGHT",
                          "ICEACTION_BUBBLE_BACK",
                          "ICEACTION_BUBBLE_FORTH",
                          "ICEACTION_SHAKE_MAG_DEC",
                          "ICEACTION_SHAKE_MAG_INC",
                          "ICEACTION_SHAKE_FRQ_DEC",
                          "ICEACTION_SHAKE_FRQ_INC",
                          "ICEACTION_SHUTTLE_LEFT",
                          "ICEACTION_SHUTTLE_RIGHT",
                          "ICEACTION_SHUTTLE_UP",
                          "ICEACTION_SHUTTLE_DOWN",
                          "ICEACTION_SHUTTLE_SPEED_DEC",
                          "ICEACTION_SHUTTLE_SPEED_INC",
                          "ACTION_EXITAPPLICATION",
                          "ACTION_PLUGGED",
                          "ACTION_UNPLUGGED",
                          "ACTION_FLUSH",
                          "MAX_ACTIONID"};

#endif
#ifdef GAME_CARBON
char* ActionIDStr[MAX_ACTIONID] = {"NULL_ACTION",
                                   "GAMEACTION_STEERLEFT",
                                   "GAMEACTION_STEERRIGHT",
                                   "GAMEACTION_OVERSTEER",
                                   "GAMEACTION_OVERSTEERLEFT",
                                   "GAMEACTION_OVERSTEERRIGHT",
                                   "GAMEACTION_TURNLEFT",
                                   "GAMEACTION_TURNRIGHT",
                                   "GAMEACTION_GAS",
                                   "GAMEACTION_BRAKE",
                                   "GAMEACTION_HANDBRAKE",
                                   "GAMEACTION_GAMEBREAKER",
                                   "GAMEACTION_SHIFTUP",
                                   "GAMEACTION_SHIFTDOWN",
                                   "GAMEACTION_SHIFTFIRST",
                                   "GAMEACTION_SHIFTSECOND",
                                   "GAMEACTION_SHIFTTHIRD",
                                   "GAMEACTION_SHIFTFOURTH",
                                   "GAMEACTION_SHIFTFIFTH",
                                   "GAMEACTION_SHIFTSIXTH",
                                   "GAMEACTION_SHIFTNEUTRAL",
                                   "GAMEACTION_SHIFTREVERSE",
                                   "GAMEACTION_NOS",
                                   "GAMEACTION_RESET",
                                   "GAMEACTION_CREWAGGRESSIVE",
                                   "GAMEACTION_CREWDEFENSIVE",
                                   "GAMEACTION_CREWDEFAULT",
                                   "GAMEACTION_CREWSPEED",
                                   "GAMEACTION_FORWARD",
                                   "GAMEACTION_BACK",
                                   "GAMEACTION_JUMP",
                                   "GAMEACTION_DEBUGHUMAN1",
                                   "GAMEACTION_DEBUGHUMAN2",
                                   "GAMEACTION_DEBUGHUMAN3",
                                   "GAMEACTION_DEBUGHUMAN4",
                                   "GAMEACTION_CLUTCH",
                                   "CAMERAACTION_CHANGE",
                                   "CAMERAACTION_DEBUG",
                                   "CAMERAACTION_ENABLE_ICE",
                                   "CAMERAACTION_LOOKBACK",
                                   "CAMERAACTION_PULLBACK",
                                   "FRONTENDACTION_UP",
                                   "FRONTENDACTION_DOWN",
                                   "FRONTENDACTION_LEFT",
                                   "FRONTENDACTION_RIGHT",
                                   "FRONTENDACTION_RUP",
                                   "FRONTENDACTION_RDOWN",
                                   "FRONTENDACTION_RLEFT",
                                   "FRONTENDACTION_RRIGHT",
                                   "FRONTENDACTION_ACCEPT",
                                   "FRONTENDACTION_CANCEL",
                                   "FRONTENDACTION_CANCEL_ALT",
                                   "FRONTENDACTION_START",
                                   "FRONTENDACTION_BUTTON0",
                                   "FRONTENDACTION_BUTTON1",
                                   "FRONTENDACTION_BUTTON2",
                                   "FRONTENDACTION_BUTTON3",
                                   "FRONTENDACTION_BUTTON4",
                                   "FRONTENDACTION_BUTTON5",
                                   "FRONTENDACTION_LTRIGGER",
                                   "FRONTENDACTION_RTRIGGER",
                                   "HUDACTION_PAUSEREQUEST",
                                   "HUDACTION_ENGAGE_EVENT",
                                   "HUDACTION_PAD_LEFT",
                                   "HUDACTION_PAD_DOWN",
                                   "HUDACTION_PAD_RIGHT",
                                   "HUDACTION_SKIPNIS",
                                   "HUDACTION_NEXTSONG",
                                   "HUDACTION_VOIPACTIVATION",
                                   "HUDACTION_VOIPDEACTIVATION",
                                   "VOIPACTION_PUSHTOTALK",
                                   "DEBUGACTION_MOVE_UP",
                                   "DEBUGACTION_MOVE_DOWN",
                                   "DEBUGACTION_MOVE_LEFT",
                                   "DEBUGACTION_MOVE_RIGHT",
                                   "DEBUGACTION_MOVE_FORWARD",
                                   "DEBUGACTION_MOVE_BACK",
                                   "DEBUGACTION_MOVE_D_LEFT",
                                   "DEBUGACTION_MOVE_D_RIGHT",
                                   "DEBUGACTION_MOVE_D_FORWARD",
                                   "DEBUGACTION_MOVE_D_BACK",
                                   "DEBUGACTION_LOOK_UP",
                                   "DEBUGACTION_LOOK_DOWN",
                                   "DEBUGACTION_LOOK_LEFT",
                                   "DEBUGACTION_LOOK_RIGHT",
                                   "DEBUGACTION_LOOK_D_UP",
                                   "DEBUGACTION_LOOK_D_DOWN",
                                   "DEBUGACTION_LOOK_D_LEFT",
                                   "DEBUGACTION_LOOK_D_RIGHT",
                                   "DEBUGACTION_TURBO",
                                   "DEBUGACTION_SUPER_TURBO",
                                   "DEBUGACTION_DROPCAR",
                                   "DEBUGACTION_TOGGLEAI",
                                   "DEBUGACTION_TOGGLESIMSTEP",
                                   "DEBUGACTION_SIMSTEP",
                                   "DEBUGACTION_SCREENSHOT",
                                   "DEBUGACTION_STOPRECORDPATH",
                                   "DEBUGACTION_TOGGLECARCOLOUR",
                                   "DEBUGACTION_TOGGLEDEMOCAMERAS",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "unknown",
                                   "ACTION_EXITAPPLICATION",
                                   "ACTION_PLUGGED",
                                   "ACTION_UNPLUGGED",
                                   "ACTION_FLUSH"};

#endif
#ifdef GAME_PROSTREET
char* ActionIDStr[MAX_ACTIONID] = {"NULL_ACTION",
                                   "GAMEACTION_STEERLEFT",
                                   "GAMEACTION_STEERRIGHT",
                                   "GAMEACTION_OVERSTEER",
                                   "GAMEACTION_OVERSTEERLEFT",
                                   "GAMEACTION_OVERSTEERRIGHT",
                                   "GAMEACTION_TURNLEFT",
                                   "GAMEACTION_TURNRIGHT",
                                   "GAMEACTION_GAS",
                                   "GAMEACTION_GAS_SLOTCAR",
                                   "GAMEACTION_BRAKE",
                                   "GAMEACTION_HANDBRAKE",
                                   "GAMEACTION_GAMEBREAKER",
                                   "GAMEACTION_SHIFTUP",
                                   "GAMEACTION_SHIFTDOWN",
                                   "GAMEACTION_SHIFTFIRST",
                                   "GAMEACTION_SHIFTSECOND",
                                   "GAMEACTION_SHIFTTHIRD",
                                   "GAMEACTION_SHIFTFOURTH",
                                   "GAMEACTION_SHIFTFIFTH",
                                   "GAMEACTION_SHIFTSIXTH",
                                   "GAMEACTION_SHIFTNEUTRAL",
                                   "GAMEACTION_SHIFTREVERSE",
                                   "GAMEACTION_NOS",
                                   "GAMEACTION_RESET",
                                   "GAMEACTION_CREWAGGRESSIVE",
                                   "GAMEACTION_CREWDEFENSIVE",
                                   "GAMEACTION_CREWDEFAULT",
                                   "GAMEACTION_CREWSPEED",
                                   "GAMEACTION_FORWARD",
                                   "GAMEACTION_BACK",
                                   "GAMEACTION_JUMP",
                                   "GAMEACTION_DEBUGHUMAN1",
                                   "GAMEACTION_DEBUGHUMAN2",
                                   "GAMEACTION_DEBUGHUMAN3",
                                   "GAMEACTION_DEBUGHUMAN4",
                                   "GAMEACTION_CLUTCH",
                                   "GAMEACTION_CLUTCH_KICK",
                                   "CAMERAACTION_CHANGE",
                                   "CAMERAACTION_DEBUG",
                                   "CAMERAACTION_ENABLE_ICE",
                                   "CAMERAACTION_LOOKBACK",
                                   "CAMERAACTION_PULLBACK",
                                   "CAMERAACTION_ROT_LEFT",
                                   "CAMERAACTION_ROT_RIGHT",
                                   "CAMERAACTION_360",
                                   "FRONTENDACTION_UP",
                                   "FRONTENDACTION_DOWN",
                                   "FRONTENDACTION_LEFT",
                                   "FRONTENDACTION_RIGHT",
                                   "FRONTENDACTION_RUP",
                                   "FRONTENDACTION_RDOWN",
                                   "FRONTENDACTION_RLEFT",
                                   "FRONTENDACTION_RRIGHT",
                                   "FRONTENDACTION_ACCEPT",
                                   "FRONTENDACTION_CANCEL",
                                   "FRONTENDACTION_CANCEL_ALT",
                                   "FRONTENDACTION_START",
                                   "FRONTENDACTION_BUTTON0",
                                   "FRONTENDACTION_BUTTON1",
                                   "FRONTENDACTION_BUTTON2",
                                   "FRONTENDACTION_BUTTON3",
                                   "FRONTENDACTION_BUTTON4",
                                   "FRONTENDACTION_BUTTON5",
                                   "FRONTENDACTION_LTRIGGER",
                                   "FRONTENDACTION_RTRIGGER",
                                   "HUDACTION_PAUSEREQUEST",
                                   "HUDACTION_ENGAGE_EVENT",
                                   "HUDACTION_PAD_LEFT",
                                   "HUDACTION_PAD_DOWN",
                                   "HUDACTION_PAD_RIGHT",
                                   "HUDACTION_SKIPNIS",
                                   "HUDACTION_NEXTSONG",
                                   "HUDACTION_VOIPACTIVATION",
                                   "HUDACTION_VOIPDEACTIVATION",
                                   "HUDACTION_TOGGLE_LEADERBOARD",
                                   "HUDACTION_TOGGLE_MINIMAP",
                                   "HUDACTION_TOGGLE_BESTLINE",
                                   "HUDACTION_TOGGLE_ARROWS",
                                   "VOIPACTION_PUSHTOTALK",
                                   "DEBUGACTION_MOVE_UP",
                                   "DEBUGACTION_MOVE_DOWN",
                                   "DEBUGACTION_MOVE_LEFT",
                                   "DEBUGACTION_MOVE_RIGHT",
                                   "DEBUGACTION_MOVE_FORWARD",
                                   "DEBUGACTION_MOVE_BACK",
                                   "DEBUGACTION_MOVE_D_LEFT",
                                   "DEBUGACTION_MOVE_D_RIGHT",
                                   "DEBUGACTION_MOVE_D_FORWARD",
                                   "DEBUGACTION_MOVE_D_BACK",
                                   "DEBUGACTION_LOOK_UP",
                                   "DEBUGACTION_LOOK_DOWN",
                                   "DEBUGACTION_LOOK_LEFT",
                                   "DEBUGACTION_LOOK_RIGHT",
                                   "DEBUGACTION_LOOK_D_UP",
                                   "DEBUGACTION_LOOK_D_DOWN",
                                   "DEBUGACTION_LOOK_D_LEFT",
                                   "DEBUGACTION_LOOK_D_RIGHT",
                                   "DEBUGACTION_TURBO",
                                   "DEBUGACTION_SUPER_TURBO",
                                   "DEBUGACTION_DROPCAR",
                                   "DEBUGACTION_TOGGLEAI",
                                   "DEBUGACTION_TOGGLESIMSTEP",
                                   "DEBUGACTION_SIMSTEP",
                                   "DEBUGACTION_SCREENSHOT",
                                   "DEBUGACTION_STOPRECORDPATH",
                                   "DEBUGACTION_TOGGLECARCOLOUR",
                                   "DEBUGACTION_TOGGLEDEMOCAMERAS",
                                   "ICE_ACTION_START",
                                   "ICE_ACTION_SELECT",
                                   "ICE_ACTION_SOUTH",
                                   "ICE_ACTION_SOUTH_PRESS",
                                   "ICE_ACTION_NORTH",
                                   "ICE_ACTION_NORTH_PRESS",
                                   "ICE_ACTION_WEST",
                                   "ICE_ACTION_WEST_PRESS",
                                   "ICE_ACTION_EAST",
                                   "ICE_ACTION_EAST_PRESS",
                                   "ICE_ACTION_DOWN",
                                   "ICE_ACTION_DOWN_PRESS",
                                   "ICE_ACTION_UP",
                                   "ICE_ACTION_UP_PRESS",
                                   "ICE_ACTION_LEFT",
                                   "ICE_ACTION_LEFT_PRESS",
                                   "ICE_ACTION_RIGHT",
                                   "ICE_ACTION_RIGHT_PRESS",
                                   "ICE_ACTION_L1",
                                   "ICE_ACTION_L1_PRESS",
                                   "ICE_ACTION_R1",
                                   "ICE_ACTION_R1_PRESS",
                                   "ICE_ACTION_L2",
                                   "ICE_ACTION_L2_PRESS",
                                   "ICE_ACTION_R2",
                                   "ICE_ACTION_R2_PRESS",
                                   "ICE_ACTION_L3",
                                   "ICE_ACTION_L3_PRESS",
                                   "ICE_ACTION_R3",
                                   "ICE_ACTION_R3_RELEASE",
                                   "ICE_ACTION_LEFTSTICK_LEFT",
                                   "ICE_ACTION_LEFTSTICK_RIGHT",
                                   "ICE_ACTION_LEFTSTICK_DOWN",
                                   "ICE_ACTION_LEFTSTICK_UP",
                                   "ICE_ACTION_RIGHTSTICK_LEFT",
                                   "ICE_ACTION_RIGHTSTICK_RIGHT",
                                   "ICE_ACTION_RIGHTSTICK_DOWN",
                                   "ICE_ACTION_RIGHTSTICK_UP",
                                   "ACTION_EXITAPPLICATION",
                                   "ACTION_PLUGGED",
                                   "ACTION_UNPLUGGED",
                                   "ACTION_FLUSH"};

#endif
#ifdef GAME_UC
char* ActionIDStr[MAX_ACTIONID] = {"NULL_ACTION",
                                   "GAMEACTION_STEERLEFT",
                                   "GAMEACTION_STEERRIGHT",
                                   "GAMEACTION_OVERSTEER",
                                   "GAMEACTION_OVERSTEERLEFT",
                                   "GAMEACTION_OVERSTEERRIGHT",
                                   "GAMEACTION_TURNLEFT",
                                   "GAMEACTION_TURNRIGHT",
                                   "GAMEACTION_GAS",
                                   "GAMEACTION_GAS_SLOTCAR",
                                   "GAMEACTION_BRAKE",
                                   "GAMEACTION_HANDBRAKE",
                                   "GAMEACTION_GAMEBREAKER",
                                   "GAMEACTION_SHIFTUP",
                                   "GAMEACTION_SHIFTDOWN",
                                   "GAMEACTION_SHIFTFIRST",
                                   "GAMEACTION_SHIFTSECOND",
                                   "GAMEACTION_SHIFTTHIRD",
                                   "GAMEACTION_SHIFTFOURTH",
                                   "GAMEACTION_SHIFTFIFTH",
                                   "GAMEACTION_SHIFTSIXTH",
                                   "GAMEACTION_SHIFTNEUTRAL",
                                   "GAMEACTION_SHIFTREVERSE",
                                   "GAMEACTION_NOS",
                                   "GAMEACTION_RESET",
                                   "GAMEACTION_HYDRAULICALL",
                                   "GAMEACTION_HYDRAULICFRONT",
                                   "GAMEACTION_HYDRAULICLEFT",
                                   "GAMEACTION_HYDRAULICREAR",
                                   "GAMEACTION_HYDRAULICRIGHT",
                                   "GAMEACTION_CREWAGGRESSIVE",
                                   "GAMEACTION_CREWDEFENSIVE",
                                   "GAMEACTION_CREWDEFAULT",
                                   "GAMEACTION_CREWSPEED",
                                   "GAMEACTION_FORWARD",
                                   "GAMEACTION_BACK",
                                   "GAMEACTION_JUMP",
                                   "GAMEACTION_DEBUGHUMAN1",
                                   "GAMEACTION_DEBUGHUMAN2",
                                   "GAMEACTION_DEBUGHUMAN3",
                                   "GAMEACTION_DEBUGHUMAN4",
                                   "GAMEACTION_CLUTCH",
                                   "GAMEACTION_CLUTCH_KICK",
                                   "CAMERAACTION_CHANGE",
                                   "CAMERAACTION_DEBUG",
                                   "CAMERAACTION_ENABLE_ICE",
                                   "CAMERAACTION_LOOKBACK",
                                   "CAMERAACTION_PULLBACK",
                                   "CAMERAACTION_ROT_LEFT",
                                   "CAMERAACTION_ROT_RIGHT",
                                   "CAMERAACTION_360",
                                   "FRONTENDACTION_UP",
                                   "FRONTENDACTION_DOWN",
                                   "FRONTENDACTION_LEFT",
                                   "FRONTENDACTION_RIGHT",
                                   "FRONTENDACTION_RUP",
                                   "FRONTENDACTION_RDOWN",
                                   "FRONTENDACTION_RLEFT",
                                   "FRONTENDACTION_RRIGHT",
                                   "FRONTENDACTION_ACCEPT",
                                   "FRONTENDACTION_CANCEL",
                                   "FRONTENDACTION_CANCEL_ALT",
                                   "FRONTENDACTION_START",
                                   "FRONTENDACTION_BUTTON0",
                                   "FRONTENDACTION_BUTTON1",
                                   "FRONTENDACTION_BUTTON2",
                                   "FRONTENDACTION_BUTTON3",
                                   "FRONTENDACTION_BUTTON4",
                                   "FRONTENDACTION_BUTTON5",
                                   "FRONTENDACTION_LTRIGGER",
                                   "FRONTENDACTION_RTRIGGER",
                                   "HUDACTION_PAUSEREQUEST",
                                   "HUDACTION_MESSENGER",
                                   "HUDACTION_RACE_NOW",
                                   "HUDACTION_ENGAGE_EVENT",
                                   "HUDACTION_PAD_LEFT",
                                   "HUDACTION_PAD_DOWN",
                                   "HUDACTION_PAD_RIGHT",
                                   "HUDACTION_SKIPNIS",
                                   "HUDACTION_NEXTSONG",
                                   "HUDACTION_VOIPACTIVATION",
                                   "HUDACTION_VOIPDEACTIVATION",
                                   "HUDACTION_TOGGLE_LEADERBOARD",
                                   "HUDACTION_TOGGLE_MINIMAP",
                                   "HUDACTION_TOGGLE_BESTLINE",
                                   "HUDACTION_TOGGLE_ARROWS",
                                   "VOIPACTION_PUSHTOTALK",
                                   "DEBUGACTION_MOVE_UP",
                                   "DEBUGACTION_MOVE_DOWN",
                                   "DEBUGACTION_MOVE_LEFT",
                                   "DEBUGACTION_MOVE_RIGHT",
                                   "DEBUGACTION_MOVE_FORWARD",
                                   "DEBUGACTION_MOVE_BACK",
                                   "DEBUGACTION_MOVE_D_LEFT",
                                   "DEBUGACTION_MOVE_D_RIGHT",
                                   "DEBUGACTION_MOVE_D_FORWARD",
                                   "DEBUGACTION_MOVE_D_BACK",
                                   "DEBUGACTION_LOOK_UP",
                                   "DEBUGACTION_LOOK_DOWN",
                                   "DEBUGACTION_LOOK_LEFT",
                                   "DEBUGACTION_LOOK_RIGHT",
                                   "DEBUGACTION_LOOK_D_UP",
                                   "DEBUGACTION_LOOK_D_DOWN",
                                   "DEBUGACTION_LOOK_D_LEFT",
                                   "DEBUGACTION_LOOK_D_RIGHT",
                                   "DEBUGACTION_TURBO",
                                   "DEBUGACTION_SUPER_TURBO",
                                   "DEBUGACTION_DROPCAR",
                                   "DEBUGACTION_TOGGLEAI",
                                   "DEBUGACTION_TOGGLESIMSTEP",
                                   "DEBUGACTION_SIMSTEP",
                                   "DEBUGACTION_SCREENSHOT",
                                   "DEBUGACTION_STOPRECORDPATH",
                                   "DEBUGACTION_TOGGLECARCOLOUR",
                                   "DEBUGACTION_TOGGLEDEMOCAMERAS",
                                   "DEBUGACTION_ITERCAMERATARGET",
                                   "ICE_ACTION_START",
                                   "ICE_ACTION_SELECT",
                                   "ICE_ACTION_SOUTH",
                                   "ICE_ACTION_SOUTH_PRESS",
                                   "ICE_ACTION_NORTH",
                                   "ICE_ACTION_NORTH_PRESS",
                                   "ICE_ACTION_WEST",
                                   "ICE_ACTION_WEST_PRESS",
                                   "ICE_ACTION_EAST",
                                   "ICE_ACTION_EAST_PRESS",
                                   "ICE_ACTION_DOWN",
                                   "ICE_ACTION_DOWN_PRESS",
                                   "ICE_ACTION_UP",
                                   "ICE_ACTION_UP_PRESS",
                                   "ICE_ACTION_LEFT",
                                   "ICE_ACTION_LEFT_PRESS",
                                   "ICE_ACTION_RIGHT",
                                   "ICE_ACTION_RIGHT_PRESS",
                                   "ICE_ACTION_L1",
                                   "ICE_ACTION_L1_PRESS",
                                   "ICE_ACTION_R1",
                                   "ICE_ACTION_R1_PRESS",
                                   "ICE_ACTION_L2",
                                   "ICE_ACTION_L2_PRESS",
                                   "ICE_ACTION_R2",
                                   "ICE_ACTION_R2_PRESS",
                                   "ICE_ACTION_L3",
                                   "ICE_ACTION_L3_PRESS",
                                   "ICE_ACTION_R3",
                                   "ICE_ACTION_R3_RELEASE",
                                   "ICE_ACTION_LEFTSTICK_LEFT",
                                   "ICE_ACTION_LEFTSTICK_RIGHT",
                                   "ICE_ACTION_LEFTSTICK_DOWN",
                                   "ICE_ACTION_LEFTSTICK_UP",
                                   "ICE_ACTION_RIGHTSTICK_LEFT",
                                   "ICE_ACTION_RIGHTSTICK_RIGHT",
                                   "ICE_ACTION_RIGHTSTICK_DOWN",
                                   "ICE_ACTION_RIGHTSTICK_UP",
                                   "ACTION_EXITAPPLICATION",
                                   "ACTION_PLUGGED",
                                   "ACTION_UNPLUGGED",
                                   "ACTION_FLUSH"};

#endif
#ifdef GAME_WORLD
char* ActionIDStr[MAX_ACTIONID] = {"NULL_ACTION",
                                   "GAMEACTION_STEERLEFT",
                                   "GAMEACTION_STEERRIGHT",
                                   "GAMEACTION_TURNLEFT",
                                   "GAMEACTION_TURNRIGHT",
                                   "Unknown_5",
                                   "Unknown_6",
                                   "GAMEACTION_GAS",
                                   "GAMEACTION_BRAKE",
                                   "GAMEACTION_HANDBRAKE",
                                   "GAMEACTION_GAMEBREAKER",
                                   "GAMEACTION_SHIFTUP",
                                   "GAMEACTION_SHIFTDOWN",
                                   "GAMEACTION_RESET",
                                   "GAMEACTION_POWERUP_0",
                                   "GAMEACTION_POWERUP_1",
                                   "GAMEACTION_POWERUP_2",
                                   "GAMEACTION_POWERUP_3",
                                   "GAMEACTION_OPTIONS_SHORTCUT",
                                   "GAMEACTION_PROFILE_SHORTCUT",
                                   "GAMEACTION_SAFEHOUSE_SHORTCUT",
                                   "GAMEACTION_WORLDMAP_SHORTCUT",
                                   "GAMEACTION_FRIENDSLIST_SHORTCUT",
                                   "GAMEACTION_POWERUPCHANGEPAGE_UP",
                                   "GAMEACTION_POWERUPCHANGEPAGE_DOWN",
                                   "GAMEACTION_HELP_SHORTCUT",
                                   "GAMEACTION_CHAT_SHORTCUT",
                                   "GAMEACTION_FORWARD",
                                   "GAMEACTION_BACK",
                                   "GAMEACTION_JUMP",
                                   "GAMEACTION_DEBUGHUMAN1",
                                   "GAMEACTION_DEBUGHUMAN2",
                                   "GAMEACTION_DEBUGHUMAN3",
                                   "GAMEACTION_DEBUGHUMAN4",
                                   "GAMEACTION_CLUTCH",
                                   "GAMEACTION_TOGGLE_AUTODRIVE",
                                   "GAMEACTION_TOGGLE_COPLIGHTS",
                                   "GAMEACTION_TOGGLE_AUDIO",
                                   "GAMEACTION_HYDRAULICS_BACK",
                                   "GAMEACTION_HYDRAULICS_FRONT",
                                   "GAMEACTION_HYDRAULICS_LEFT",
                                   "GAMEACTION_HYDRAULICS_RIGHT",
                                   "CAMERAACTION_CHANGE",
                                   "CAMERAACTION_DEBUG",
                                   "CAMERAACTION_ENABLE_ICE",
                                   "CAMERAACTION_LOOKBACK",
                                   "CAMERAACTION_PULLBACK",
                                   "CAMERAACTION_TOGGLE",
                                   "CAMERAACTION_ROT_UP",
                                   "CAMERAACTION_ROT_DOWN",
                                   "CAMERAACTION_ROT_LEFT",
                                   "CAMERAACTION_ROT_RIGHT",
                                   "CAMERAACTION_MOVE_UP",
                                   "CAMERAACTION_MOVE_DOWN",
                                   "CAMERAACTION_MOVE_LEFT",
                                   "CAMERAACTION_MOVE_RIGHT",
                                   "CAMERAACTION_MOVE_FORWARD",
                                   "CAMERAACTION_MOVE_BACKWARD",
                                   "CAMERAACTION_FOV_LARGER",
                                   "CAMERAACTION_FOV_SMALLER",
                                   "CAMERAACTION_CRANE_DOWN",
                                   "CAMERAACTION_CRANE_UP",
                                   "CAMERAACTION_DOLLY_LEFT",
                                   "CAMERAACTION_DOLLY_RIGHT",
                                   "CAMERAACTION_TURBOSPEED",
                                   "CAMERAACTION_CHANGE_CAR",
                                   "FRONTENDACTION_UP",
                                   "FRONTENDACTION_DOWN",
                                   "FRONTENDACTION_LEFT",
                                   "FRONTENDACTION_RIGHT",
                                   "FRONTENDACTION_RUP",
                                   "FRONTENDACTION_RDOWN",
                                   "FRONTENDACTION_RLEFT",
                                   "FRONTENDACTION_RRIGHT",
                                   "FRONTENDACTION_ACCEPT",
                                   "FRONTENDACTION_CANCEL",
                                   "FRONTENDACTION_CANCEL_ALT",
                                   "FRONTENDACTION_START",
                                   "FRONTENDACTION_BUTTON0",
                                   "FRONTENDACTION_BUTTON1",
                                   "FRONTENDACTION_BUTTON2",
                                   "FRONTENDACTION_BUTTON3",
                                   "FRONTENDACTION_BUTTON4",
                                   "FRONTENDACTION_BUTTON5",
                                   "FRONTENDACTION_LTRIGGER",
                                   "FRONTENDACTION_RTRIGGER",
                                   "FRONTENDACTION_PGUP",
                                   "FRONTENDACTION_PGDOWN",
                                   "HUDACTION_PAUSEREQUEST",
                                   "HUDACTION_ENGAGE_EVENT",
                                   "HUDACTION_PAD_LEFT",
                                   "HUDACTION_PAD_DOWN",
                                   "HUDACTION_PAD_RIGHT",
                                   "HUDACTION_SKIPNIS",
                                   "HUDACTION_SCREENSHOT",
                                   "HUDACTION_NEXTSONG",
                                   "HUDACTION_VOIPACTIVATION",
                                   "HUDACTION_VOIPDEACTIVATION",
                                   "VOIPACTION_PUSHTOTALK",
                                   "DEBUGACTION_MOVE_UP",
                                   "DEBUGACTION_MOVE_DOWN",
                                   "DEBUGACTION_MOVE_LEFT",
                                   "DEBUGACTION_MOVE_RIGHT",
                                   "DEBUGACTION_MOVE_FORWARD",
                                   "DEBUGACTION_MOVE_BACK",
                                   "DEBUGACTION_LOOK_UP",
                                   "DEBUGACTION_LOOK_DOWN",
                                   "DEBUGACTION_LOOK_LEFT",
                                   "DEBUGACTION_LOOK_RIGHT",
                                   "DEBUGACTION_TURBO",
                                   "DEBUGACTION_SUPER_TURBO",
                                   "DEBUGACTION_CAMERA_SAVE",
                                   "DEBUGACTION_CAMERA_LOAD",
                                   "DEBUGACTION_DEBUGDRAW_DRAWTICK_DEC",
                                   "DEBUGACTION_DEBUGDRAW_DRAWTICK_INC",
                                   "DEBUGACTION_DEBUGDRAW_DRAWTICKRANGE_DEC",
                                   "DEBUGACTION_DEBUGDRAW_DRAWTICKRANGE_INC",
                                   "DEBUGACTION_DROPCAR",
                                   "DEBUGACTION_LOCKTOCAR",
                                   "DEBUGACTION_TOGGLESIMSTEP",
                                   "DEBUGACTION_SIMSTEP",
                                   "DEBUGACTION_TOGGLESIMPAUSE",
                                   "DEBUGACTION_SCREENSHOT",
                                   "DEBUGACTION_STOPRECORDPATH",
                                   "DEBUGACTION_TOGGLECARCOLOUR",
                                   "DEBUGACTION_TOGGLEDEMOCAMERAS",
                                   "DEBUGACTION_TOGGLEMENU",
                                   "Unknown_127",
                                   "Unknown_128",
                                   "Unknown_129",
                                   "Unknown_130",
                                   "Unknown_131",
                                   "Unknown_132",
                                   "Unknown_133",
                                   "Unknown_134",
                                   "Unknown_135",
                                   "Unknown_136",
                                   "Unknown_137",
                                   "Unknown_138",
                                   "Unknown_139",
                                   "Unknown_140",
                                   "Unknown_141",
                                   "Unknown_142",
                                   "Unknown_143",
                                   "Unknown_144",
                                   "Unknown_145",
                                   "Unknown_146",
                                   "Unknown_147",
                                   "Unknown_148",
                                   "Unknown_149",
                                   "Unknown_150",
                                   "Unknown_151",
                                   "Unknown_152",
                                   "Unknown_153",
                                   "Unknown_154",
                                   "Unknown_155",
                                   "Unknown_156",
                                   "Unknown_157",
                                   "Unknown_158",
                                   "Unknown_159",
                                   "Unknown_160",
                                   "Unknown_161",
                                   "Unknown_162",
                                   "Unknown_163",
                                   "ACTION_EXITAPPLICATION",
                                   "ACTION_PLUGGED",
                                   "ACTION_UNPLUGGED",
                                   "ACTION_FLUSH"};

#endif
bool bIsActionAnalog(ActionID in) {
  switch (in) {
    case GAMEACTION_STEERLEFT:
    case GAMEACTION_STEERRIGHT:
    case GAMEACTION_TURNLEFT:
    case GAMEACTION_TURNRIGHT:
    case GAMEACTION_GAS:
    case GAMEACTION_BRAKE:
    case FRONTENDACTION_RUP:
    case FRONTENDACTION_RDOWN:
    case FRONTENDACTION_RLEFT:
    case FRONTENDACTION_RRIGHT:
    case DEBUGACTION_MOVE_UP:
    case DEBUGACTION_MOVE_DOWN:
    case DEBUGACTION_MOVE_LEFT:
    case DEBUGACTION_MOVE_RIGHT:
    case DEBUGACTION_MOVE_FORWARD:
    case DEBUGACTION_MOVE_BACK:
    case DEBUGACTION_LOOK_UP:
    case DEBUGACTION_LOOK_DOWN:
    case DEBUGACTION_LOOK_LEFT:
    case DEBUGACTION_LOOK_RIGHT:
    case DEBUGACTION_TURBO:
    case DEBUGACTION_SUPER_TURBO:
    case FRONTENDACTION_LTRIGGER:
    case FRONTENDACTION_RTRIGGER:
#ifdef GAME_DEBUG_VERSION
    case GAMEACTION_GAS_SLOTCAR:
    case ICE_ACTION_LEFTSTICK_LEFT:
    case ICE_ACTION_LEFTSTICK_RIGHT:
    case ICE_ACTION_LEFTSTICK_DOWN:
    case ICE_ACTION_LEFTSTICK_UP:
    case ICE_ACTION_RIGHTSTICK_LEFT:
    case ICE_ACTION_RIGHTSTICK_RIGHT:
    case ICE_ACTION_RIGHTSTICK_DOWN:
    case ICE_ACTION_RIGHTSTICK_UP:
#endif
      return true;
  }

  return false;
}

bool bIsActionFrontEnd(ActionID in) {
  switch (in) {
    case FRONTENDACTION_UP:
    case FRONTENDACTION_DOWN:
    case FRONTENDACTION_LEFT:
    case FRONTENDACTION_RIGHT:
    case FRONTENDACTION_RUP:
    case FRONTENDACTION_RDOWN:
    case FRONTENDACTION_RLEFT:
    case FRONTENDACTION_RRIGHT:
    case FRONTENDACTION_ACCEPT:
    case FRONTENDACTION_CANCEL:
    case FRONTENDACTION_CANCEL_ALT:
    case FRONTENDACTION_START:
    case FRONTENDACTION_BUTTON0:
    case FRONTENDACTION_BUTTON1:
    case FRONTENDACTION_BUTTON2:
    case FRONTENDACTION_BUTTON3:
    case FRONTENDACTION_BUTTON4:
    case FRONTENDACTION_BUTTON5:
    case FRONTENDACTION_LTRIGGER:
    case FRONTENDACTION_RTRIGGER:
      return true;
  }
  return false;
}

bool bIsActionHUD(ActionID in) {
  switch (in) {
    case HUDACTION_PAUSEREQUEST:
    case HUDACTION_ENGAGE_EVENT:
    case HUDACTION_PAD_LEFT:
    case HUDACTION_PAD_DOWN:
    case HUDACTION_PAD_RIGHT:
    case HUDACTION_SKIPNIS:
    case HUDACTION_NEXTSONG:
#ifndef GAME_MW
    case HUDACTION_VOIPACTIVATION:
    case HUDACTION_VOIPDEACTIVATION:
#endif
      return true;
  }
  return false;
}

bool bIsActionFrontEndNavigation(ActionID in) {
  switch (in) {
    case FRONTENDACTION_UP:
    case FRONTENDACTION_DOWN:
    case FRONTENDACTION_LEFT:
    case FRONTENDACTION_RIGHT:
      return true;
  }
  return false;
}

bool bIsActionRelativePosition(ActionID in) {
  switch (in) {
    case FRONTENDACTION_LTRIGGER:
    case FRONTENDACTION_RTRIGGER:
      return true;
  }
  return false;
}

bool bIsActionAnalogAxis(ActionID in) {
  switch (in) {
    case GAMEACTION_STEERLEFT:
    case GAMEACTION_STEERRIGHT:
    case GAMEACTION_TURNLEFT:
    case GAMEACTION_TURNRIGHT:
    case GAMEACTION_GAS:
    case GAMEACTION_BRAKE:
    case FRONTENDACTION_RUP:
    case FRONTENDACTION_RDOWN:
    case FRONTENDACTION_RLEFT:
    case FRONTENDACTION_RRIGHT:
    case FRONTENDACTION_LTRIGGER:
    case FRONTENDACTION_RTRIGGER:
    case DEBUGACTION_MOVE_UP:
    case DEBUGACTION_MOVE_DOWN:
    case DEBUGACTION_MOVE_LEFT:
    case DEBUGACTION_MOVE_RIGHT:
    case DEBUGACTION_MOVE_FORWARD:
    case DEBUGACTION_MOVE_BACK:
    case DEBUGACTION_LOOK_UP:
    case DEBUGACTION_LOOK_DOWN:
    case DEBUGACTION_LOOK_LEFT:
    case DEBUGACTION_LOOK_RIGHT:
    case DEBUGACTION_TURBO:
    case DEBUGACTION_SUPER_TURBO:
      return true;
  }
  return false;
}

bool bIsActionDigitalButton(ActionID in) {
  switch (in) {
    case GAMEACTION_GAMEBREAKER:
    case CAMERAACTION_CHANGE:
    case GAMEACTION_RESET:
    case DEBUGACTION_DROPCAR:
    case CAMERAACTION_DEBUG:
    case GAMEACTION_SHIFTUP:
    case GAMEACTION_SHIFTDOWN:
    case GAMEACTION_TURNLEFT:
    case GAMEACTION_TURNRIGHT:
    case HUDACTION_PAUSEREQUEST:
    case HUDACTION_ENGAGE_EVENT:
    case HUDACTION_PAD_LEFT:
    case HUDACTION_PAD_DOWN:
    case HUDACTION_PAD_RIGHT:
    case HUDACTION_SKIPNIS:
    case HUDACTION_NEXTSONG:
#ifndef GAME_MW
    case HUDACTION_VOIPACTIVATION:
    case HUDACTION_VOIPDEACTIVATION:
#ifndef GAME_WORLD
    case GAMEACTION_CREWAGGRESSIVE:
    case GAMEACTION_CREWDEFENSIVE:
    case GAMEACTION_CREWDEFAULT:
    case GAMEACTION_CREWSPEED:
#endif
#ifdef GAME_PROSTREET
    case HUDACTION_TOGGLE_LEADERBOARD:
    case HUDACTION_TOGGLE_MINIMAP:
    case HUDACTION_TOGGLE_BESTLINE:
    case HUDACTION_TOGGLE_ARROWS:
#endif
#ifdef GAME_WORLD
    case GAMEACTION_TOGGLE_COPLIGHTS:
    case GAMEACTION_TOGGLE_AUTODRIVE:
    case GAMEACTION_TOGGLE_AUDIO:
    case GAMEACTION_POWERUP_0:
    case GAMEACTION_POWERUP_1:
    case GAMEACTION_POWERUP_2:
    case GAMEACTION_POWERUP_3:
#endif
#endif
      return true;
  }
  return false;
}

bool bIsActionDebug(ActionID in) {
  switch (in) {
    case DEBUGACTION_MOVE_UP:
    case DEBUGACTION_MOVE_DOWN:
    case DEBUGACTION_MOVE_LEFT:
    case DEBUGACTION_MOVE_RIGHT:
    case DEBUGACTION_MOVE_FORWARD:
    case DEBUGACTION_MOVE_BACK:
    case DEBUGACTION_LOOK_UP:
    case DEBUGACTION_LOOK_DOWN:
    case DEBUGACTION_LOOK_LEFT:
    case DEBUGACTION_LOOK_RIGHT:
    case DEBUGACTION_TURBO:
    case DEBUGACTION_SUPER_TURBO:
    case DEBUGACTION_DROPCAR:
    case DEBUGACTION_TOGGLESIMSTEP:
    case DEBUGACTION_SIMSTEP:
    case DEBUGACTION_SCREENSHOT:
    case DEBUGACTION_STOPRECORDPATH:
    case DEBUGACTION_TOGGLECARCOLOUR:
    case DEBUGACTION_TOGGLEDEMOCAMERAS:
#ifdef GAME_WORLD
    case DEBUGACTION_TOGGLEMENU:
    case DEBUGACTION_TOGGLESIMPAUSE:
    case DEBUGACTION_LOCKTOCAR:
    case DEBUGACTION_DEBUGDRAW_DRAWTICK_DEC:
    case DEBUGACTION_DEBUGDRAW_DRAWTICK_INC:
    case DEBUGACTION_DEBUGDRAW_DRAWTICKRANGE_DEC:
    case DEBUGACTION_DEBUGDRAW_DRAWTICKRANGE_INC:
    case DEBUGACTION_CAMERA_SAVE:
    case DEBUGACTION_CAMERA_LOAD:
#else
    case DEBUGACTION_LOOK_D_UP:
    case DEBUGACTION_LOOK_D_DOWN:
    case DEBUGACTION_LOOK_D_LEFT:
    case DEBUGACTION_LOOK_D_RIGHT:
    case DEBUGACTION_MOVE_D_LEFT:
    case DEBUGACTION_MOVE_D_RIGHT:
    case DEBUGACTION_MOVE_D_FORWARD:
    case DEBUGACTION_MOVE_D_BACK:
    case DEBUGACTION_TOGGLEAI:
#endif
#ifdef GAME_DEBUG_VERSION
    case ICEACTION_GRAB_LEFT:
    case ICEACTION_GRAB_RIGHT:
    case ICEACTION_LEFT:
    case ICEACTION_RIGHT:
    case ICEACTION_FAST_LEFT:
    case ICEACTION_FAST_RIGHT:
    case ICEACTION_UP:
    case ICEACTION_DOWN:
    case ICEACTION_HELP:
    case ICEACTION_UNDO:
    case ICEACTION_PLAY:
    case ICEACTION_HIDE:
    case ICEACTION_SCREENSHOT:
    case ICEACTION_SCREENSHOT_STREAM:
    case ICEACTION_INSERT:
    case ICEACTION_CUT:
    case ICEACTION_COPY:
    case ICEACTION_PASTE:
    case ICEACTION_SELECT:
    case ICEACTION_CANCEL:
    case ICEACTION_ALT_1:
    case ICEACTION_CLIP_IN:
    case ICEACTION_CLIP_OUT:
    case ICEACTION_SIMSPEED_INC:
    case ICEACTION_SIMSPEED_DEC:
    case ICEACTION_DUTCH_LEFT:
    case ICEACTION_DUTCH_RIGHT:
    case ICEACTION_ZOOM_IN:
    case ICEACTION_ZOOM_OUT:
    case ICEACTION_LETTERBOX_IN:
    case ICEACTION_LETTERBOX_OUT:
    case ICEACTION_BUBBLE_IN:
    case ICEACTION_BUBBLE_OUT:
    case ICEACTION_BUBBLE_UP:
    case ICEACTION_BUBBLE_DOWN:
    case ICEACTION_BUBBLE_TILT_UP:
    case ICEACTION_BUBBLE_TILT_DOWN:
    case ICEACTION_BUBBLE_SPIN_LEFT:
    case ICEACTION_BUBBLE_SPIN_RIGHT:
    case ICEACTION_BUBBLE_LEFT:
    case ICEACTION_BUBBLE_RIGHT:
    case ICEACTION_BUBBLE_BACK:
    case ICEACTION_BUBBLE_FORTH:
    case ICEACTION_SHAKE_MAG_DEC:
    case ICEACTION_SHAKE_MAG_INC:
    case ICEACTION_SHAKE_FRQ_DEC:
    case ICEACTION_SHAKE_FRQ_INC:
    case ICEACTION_SHUTTLE_LEFT:
    case ICEACTION_SHUTTLE_RIGHT:
    case ICEACTION_SHUTTLE_UP:
    case ICEACTION_SHUTTLE_DOWN:
    case ICEACTION_SHUTTLE_SPEED_DEC:
    case ICEACTION_SHUTTLE_SPEED_INC:
#endif
    case CAMERAACTION_DEBUG:
    case CAMERAACTION_ENABLE_ICE:
      return true;
  }

  return false;
}

bool bIsActionTextureBindable(ActionID in) {
  switch (in) {
    case FRONTENDACTION_UP:
    case FRONTENDACTION_DOWN:
    case FRONTENDACTION_LEFT:
    case FRONTENDACTION_RIGHT:
    case FRONTENDACTION_RUP:
    case FRONTENDACTION_START:
    case FRONTENDACTION_ACCEPT:
    case FRONTENDACTION_CANCEL:
    case FRONTENDACTION_BUTTON0:
    case FRONTENDACTION_BUTTON1:
    case FRONTENDACTION_BUTTON2:
    case FRONTENDACTION_BUTTON3:
    case FRONTENDACTION_BUTTON4:
    case FRONTENDACTION_BUTTON5:
    case FRONTENDACTION_LTRIGGER:
    case FRONTENDACTION_RTRIGGER:
    case GAMEACTION_SHIFTUP:
    case GAMEACTION_SHIFTDOWN:
      return true;
  }
  return false;
}

#endif  // !GAME_UNDERGROUND

#ifdef GAME_UG
char JoyEventNames[MAX_JOY_EVENT + 1][50] = {
    "JOY_EVENT_NULL",
    "JOY_EVENT_TYPE_CHANGED",
    "JOY_EVENT_EXIT_DEMO_DISC",
    "JOY_EVENT_SCROLL_UP",
    "JOY_EVENT_SCROLL_DOWN",
    "JOY_EVENT_SCROLL_LEFT",
    "JOY_EVENT_SCROLL_RIGHT",
    "JOY_EVENT_START",
    "JOY_EVENT_SELECT",
    "JOY_EVENT_MENU_SELECT",
    "JOY_EVENT_MENU_CANCEL",
    "JOY_EVENT_MENU_BACK",
    "JOY_EVENT_MENU_HELP",
    "JOY_EVENT_MENU_L1",
    "JOY_EVENT_MENU_L2",
    "JOY_EVENT_MENU_R1",
    "JOY_EVENT_MENU_R2",
    "JOY_EVENT_MENU_SQUARE",
    "JOY_EVENT_MENU_ANALOG_LEFT",
    "JOY_EVENT_MENU_ANALOG_RIGHT",
    "JOY_EVENT_MENU_ANALOG_UP",
    "JOY_EVENT_MENU_ANALOG_DOWN",
    "JOY_EVENT_FENG_START",
    "JOY_EVENT_FENG_BUTTON0",
    "JOY_EVENT_FENG_BUTTON1",
    "JOY_EVENT_FENG_BUTTON2",
    "JOY_EVENT_FENG_BUTTON3",
    "JOY_EVENT_FENG_LTRIGGER",
    "JOY_EVENT_FENG_RTRIGGER",
    "JOY_EVENT_FENG_SELECT",
    "JOY_EVENT_FENG_SELECT_ALTERNATE",
    "JOY_EVENT_FENG_CANCEL",
    "JOY_EVENT_FENG_CANCEL_ALTERNATE",
    "JOY_EVENT_FENG_HELP",
    "JOY_EVENT_FENG_PAD_UP",
    "JOY_EVENT_FENG_PAD_DOWN",
    "JOY_EVENT_FENG_PAD_LEFT",
    "JOY_EVENT_FENG_PAD_RIGHT",
    "JOY_EVENT_FENG_PAD_UP_ALTERNATE",
    "JOY_EVENT_FENG_PAD_DOWN_ALTERNATE",
    "JOY_EVENT_FENG_PAD_LEFT_ALTERNATE",
    "JOY_EVENT_FENG_PAD_RIGHT_ALTERNATE",
    "JOY_EVENT_FENG_QUIT",
    "JOY_EVENT_FENG_RACEOPTIONS",
    "Unknown_0x2C",
    "JOY_EVENT_SKIP_DEMO_MODE",
    "JOY_EVENT_MAIN_MENU_SELECT_TRANSMISSION",
    "JOY_EVENT_ANY",
    "JOY_EVENT_EXIT_EDIT_TEXT",
    "JOY_EVENT_CLEAR_TEXT",
    "JOY_EVENT_CANCEL_EDIT_TEXT",
    "JOY_EVENT_DELETE_CHAR",
    "JOY_EVENT_ROTATE_LETTER_UP",
    "JOY_EVENT_ROTATE_LETTER_DOWN",
    "JOY_EVENT_PREV_CHAR",
    "JOY_EVENT_NEXT_CHAR",
    "JOY_EVENT_CARSEL_CHANGE_MODE",
    "JOY_EVENT_CARSEL_UNLOCK",
    "JOY_EVENT_CARSEL_ORBIT_LEFT",
    "JOY_EVENT_CARSEL_ORBIT_RIGHT",
    "JOY_EVENT_CARSEL_ORBIT_UP",
    "JOY_EVENT_CARSEL_ORBIT_DOWN",
    "JOY_EVENT_CARSEL_ORBIT_IN",
    "JOY_EVENT_CARSEL_ORBIT_OUT",
    "JOY_EVENT_CARSEL_ORBIT_LEFTRIGHT",
    "JOY_EVENT_CARSEL_ORBIT_UPDOWN",
    "JOY_EVENT_CARSEL_ORBIT_INOUT",
    "Unknown_0x43",
    "JOY_EVENT_CHANGE_DEBUG_CAMERA",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_RIGHT",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_LEFT",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_UP",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_DOWN",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_CLOSER",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_FARTHER",
    "JOY_EVENT_DEBUG_CAMERA_NEXT_CAR",
    "JOY_EVENT_DEBUG_CAMERA_PREV_CAR",
    "JOY_EVENT_DEBUG_CAMERA_FOLLOW_CHOPPER",
    "JOY_EVENT_DEBUG_CAMERA_TURN_LEFT",
    "JOY_EVENT_DEBUG_CAMERA_TURN_RIGHT",
    "JOY_EVENT_DEBUG_CAMERA_TURN_DOWN",
    "JOY_EVENT_DEBUG_CAMERA_TURN_UP",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_DOWN",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_UP",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_FWD",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_BACK",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_LEFT",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_RIGHT",
    "JOY_EVENT_DEBUG_CAMERA_TURBO",
    "JOY_EVENT_DEBUG_CAMERA_SUPER_TURBO",
    "JOY_EVENT_DEBUG_CAMERA_SLOW",
    "JOY_EVENT_DEBUG_CAMERA_FORWARD_BLINK",
    "JOY_EVENT_DEBUG_CAMERA_BACKWARD_BLINK",
    "JOY_EVENT_DEBUG_CAMERA_DROP_CAR",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_LEFTRIGHT",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_UPDOWN",
    "JOY_EVENT_DEBUG_CAMERA_STRAFE_LEFTRIGHT",
    "JOY_EVENT_DEBUG_CAMERA_INOUT",
    "JOY_EVENT_TRACKROUTE_EDITOR_TURN_LEFTRIGHT",
    "JOY_EVENT_TRACKROUTE_EDITOR_TURN_UPDOWN",
    "JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_FORWARDBACKWARD",
    "JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_LEFTRIGHT",
    "JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_TURBO",
    "JOY_EVENT_TRACKROUTE_EDITOR_MOVE_UP",
    "JOY_EVENT_TRACKROUTE_EDITOR_MOVE_DOWN",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_UP",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_DOWN",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_LEFT",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_RIGHT",
    "JOY_EVENT_TRACKROUTE_EDITOR_NEXT_ROUTE",
    "JOY_EVENT_TRACKROUTE_EDITOR_NEXT_POINT",
    "JOY_EVENT_TRACKROUTE_EDITOR_PREV_POINT",
    "JOY_EVENT_TRACKROUTE_EDITOR_RESET_POS",
    "JOY_EVENT_TRACKROUTE_EDITOR_EDIT_RIGHT_WIDTH",
    "JOY_EVENT_TRACKROUTE_EDITOR_EDIT_LEFT_WIDTH",
    "JOY_EVENT_TRACKROUTE_EDITOR_EDIT_POSITION",
    "JOY_EVENT_TOGGLE_TRACK_MARKER",
    "JOY_EVENT_CAMERA_POV_CHANGE",
    "JOY_EVENT_CAMERA_LOOKBACK",
    "JOY_EVENT_ZONE_FREEZE",
    "JOY_EVENT_ZONE_PREVIEW",
    "JOY_EVENT_ZONE_SLOMO",
    "JOY_EVENT_ZONE_WARP",
    "JOY_EVENT_REPLAY_SKIP",
    "JOY_EVENT_REPLAY_PAUSE",
    "JOY_EVENT_REPLAY_PREVIOUS_MARKER",
    "JOY_EVENT_REPLAY_NEXT_MARKER",
    "JOY_EVENT_REPLAY_PLAYBACK_SPEED",
    "JOY_EVENT_REPLAY_CAMERA_ANGLE",
    "JOY_EVENT_REPLAY_CAMERA_TARGET",
    "JOY_EVENT_REPLAY_HELP",
    "JOY_EVENT_REPLAY_HIDE_TIMELINE",
    "JOY_EVENT_REPLAY_MARK_DEMO",
    "JOY_EVENT_REPLAY_TEST_DEMO",
    "JOY_EVENT_REPLAY_ERASE_DEMO",
    "JOY_EVENT_STEER",
    "JOY_EVENT_STEER_ANALOG",
    "JOY_EVENT_THROTTLE",
    "JOY_EVENT_THROTTLE_ANALOG",
    "JOY_EVENT_THROTTLE_ANALOG_ALTERNATE",
    "JOY_EVENT_BRAKE",
    "JOY_EVENT_BRAKE_ANALOG",
    "JOY_EVENT_BRAKE_ANALOG_ALTERNATE",
    "JOY_EVENT_REVERSE",
    "JOY_EVENT_REVERSE_ANALOG",
    "JOY_EVENT_EBRAKE",
    "JOY_EVENT_EBRAKE_ANALOG",
    "JOY_EVENT_SHIFTUP",
    "JOY_EVENT_SHIFTDOWN",
    "JOY_EVENT_SHIFTUP_ALTERNATE",
    "JOY_EVENT_SHIFTDOWN_ALTERNATE",
    "JOY_EVENT_DISENGAGE_CLUTCH",
    "JOY_EVENT_DISENGAGE_CLUTCH_ANALOG",
    "JOY_EVENT_RESET_CAR",
    "JOY_EVENT_HONK_HORN",
    "JOY_EVENT_NITRO_BOOST",
    "JOY_EVENT_NITRO_BOOST_ALTERNATE",
    "JOY_EVENT_REQUEST_ROADBLOCK",
    "JOY_EVENT_REQUEST_HELICOPTER",
    "JOY_EVENT_FIRE_ROCKET",
    "JOY_EVENT_AIM_ROCKET_LR",
    "JOY_EVENT_AIM_ROCKET_UD",
    "JOY_EVENT_AIM_ZOOM",
    "JOY_EVENT_AUTO_PILOT",
    "Unknown_0xA4",
    "Unknown_0xA5",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_LEFT",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_RIGHT",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_LEFT_ANALOG",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_RIGHT_ANALOG",
    "JOY_EVENT_CYCLE_HUD",
    "JOY_EVENT_PAUSE",
    "JOY_EVENT_SKIP"};

bool bIsEventDigitalDownOnly(unsigned int joyevent)
{
  return (joyevent == JOY_EVENT_START) 
    || (joyevent == JOY_EVENT_SELECT) 
    || (joyevent == JOY_EVENT_CAMERA_POV_CHANGE) 
    || (joyevent == JOY_EVENT_PAUSE) 
    || (joyevent == JOY_EVENT_SKIP) 
    || (joyevent == JOY_EVENT_CHANGE_DEBUG_CAMERA)
    || (joyevent == JOY_EVENT_CYCLE_HUD)
    || (joyevent == JOY_EVENT_DEBUG_CAMERA_DROP_CAR);
}

bool bIsEventAnalog(unsigned int joyevent)
{
  return joyevent == JOY_EVENT_DISENGAGE_CLUTCH_ANALOG ||
    joyevent == JOY_EVENT_EBRAKE_ANALOG ||
    joyevent == JOY_EVENT_REVERSE_ANALOG ||
    joyevent == JOY_EVENT_THROTTLE_ANALOG_ALTERNATE ||
    joyevent == JOY_EVENT_BRAKE_ANALOG ||
    joyevent == JOY_EVENT_BRAKE_ANALOG_ALTERNATE ||
    joyevent == JOY_EVENT_THROTTLE_ANALOG ||
    joyevent == JOY_EVENT_THROTTLE_ANALOG_ALTERNATE ||
    joyevent == JOY_EVENT_STEER_ANALOG ||
    joyevent == JOY_EVENT_STEER ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_TURN_LEFTRIGHT ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_TURN_UPDOWN ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_FORWARDBACKWARD ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_LEFTRIGHT ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_INOUT ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_STRAFE_LEFTRIGHT ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_ROTATE_UPDOWN ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_ROTATE_LEFTRIGHT ||
    joyevent == JOY_EVENT_CARSEL_ORBIT_INOUT ||
    joyevent == JOY_EVENT_CARSEL_ORBIT_UPDOWN ||
    joyevent == JOY_EVENT_CARSEL_ORBIT_LEFTRIGHT;
}

// for reference, the joypad config menu:
// 3 -- steer left
// 4 -- steer right
// 1 -- throttle
// 2 -- brake
// 5 -- ebrake
// 6 -- nos
// 7 -- shift up
// 8 -- shift down
// 9 -- look back
// 10 -- camera
// 14 -- reset
// 12 -- pause
#endif  // GAME_UG

#ifdef GAME_UG2
char JoyEventNames[MAX_JOY_EVENT + 1][50] = {
    "JOY_EVENT_NULL",
    "JOY_EVENT_TYPE_CHANGED",
    "JOY_EVENT_LIVE_TYPE_CHANGED",
    "JOY_EVENT_EXIT_DEMO_DISC",
    "JOY_EVENT_SCROLL_UP",
    "JOY_EVENT_SCROLL_DOWN",
    "JOY_EVENT_SCROLL_LEFT",
    "JOY_EVENT_SCROLL_RIGHT",
    "JOY_EVENT_START",
    "JOY_EVENT_SELECT",
    "JOY_EVENT_MENU_SELECT",
    "JOY_EVENT_MENU_CANCEL",
    "JOY_EVENT_MENU_BACK",
    "JOY_EVENT_MENU_HELP",
    "JOY_EVENT_MENU_L1",
    "JOY_EVENT_MENU_L2",
    "JOY_EVENT_MENU_R1",
    "JOY_EVENT_MENU_R2",
    "JOY_EVENT_MENU_SQUARE",
    "JOY_EVENT_MENU_ANALOG_LEFT",
    "JOY_EVENT_MENU_ANALOG_RIGHT",
    "JOY_EVENT_MENU_ANALOG_UP",
    "JOY_EVENT_MENU_ANALOG_DOWN",
    "JOY_EVENT_DEBUG_MENU_ACTIVATE",
    "Unknown_0x18",
    "Unknown_0x19",
    "Unknown_0x1A",
    "Unknown_0x1B",
    "Unknown_0x1C",
    "Unknown_0x1D",
    "JOY_EVENT_FENG_START",
    "JOY_EVENT_FENG_BUTTON0",
    "JOY_EVENT_FENG_BUTTON1",
    "JOY_EVENT_FENG_BUTTON2",
    "JOY_EVENT_FENG_BUTTON3",
    "JOY_EVENT_FENG_LTRIGGER",
    "JOY_EVENT_FENG_RTRIGGER",
    "JOY_EVENT_FENG_SELECT",
    "JOY_EVENT_FENG_SELECT_ALTERNATE",
    "JOY_EVENT_FENG_CANCEL",
    "JOY_EVENT_FENG_CANCEL_ALTERNATE",
    "JOY_EVENT_FENG_HELP",
    "JOY_EVENT_FENG_PAD_UP",
    "JOY_EVENT_FENG_PAD_DOWN",
    "JOY_EVENT_FENG_PAD_LEFT",
    "JOY_EVENT_FENG_PAD_RIGHT",
    "JOY_EVENT_FENG_PAD_UP_ALTERNATE",
    "JOY_EVENT_FENG_PAD_DOWN_ALTERNATE",
    "JOY_EVENT_FENG_PAD_LEFT_ALTERNATE",
    "JOY_EVENT_FENG_PAD_RIGHT_ALTERNATE",
    "JOY_EVENT_FENG_QUIT",
    "JOY_EVENT_FENG_RACEOPTIONS",
    "Unknown_0x34",
    "JOY_EVENT_SKIP_DEMO_MODE",
    "JOY_EVENT_MAIN_MENU_SELECT_TRANSMISSION",
    "JOY_EVENT_ANY",
    "JOY_EVENT_EXIT_EDIT_TEXT",
    "JOY_EVENT_CLEAR_TEXT",
    "JOY_EVENT_CANCEL_EDIT_TEXT",
    "JOY_EVENT_DELETE_CHAR",
    "JOY_EVENT_ROTATE_LETTER_UP",
    "JOY_EVENT_ROTATE_LETTER_DOWN",
    "JOY_EVENT_PREV_CHAR",
    "JOY_EVENT_NEXT_CHAR",
    "JOY_EVENT_CARSEL_CHANGE_MODE",
    "JOY_EVENT_CARSEL_UNLOCK",
    "JOY_EVENT_CARSEL_ORBIT_LEFT",
    "JOY_EVENT_CARSEL_ORBIT_RIGHT",
    "JOY_EVENT_CARSEL_ORBIT_UP",
    "JOY_EVENT_CARSEL_ORBIT_DOWN",
    "JOY_EVENT_CARSEL_ORBIT_IN",
    "JOY_EVENT_CARSEL_ORBIT_OUT",
    "JOY_EVENT_CARSEL_ORBIT_LEFTRIGHT",
    "JOY_EVENT_CARSEL_ORBIT_UPDOWN",
    "JOY_EVENT_CARSEL_ORBIT_INOUT",
    "Unknown_0x4B",
    "JOY_EVENT_CHANGE_DEBUG_CAMERA",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_RIGHT",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_LEFT",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_UP",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_DOWN",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_CLOSER",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_FARTHER",
    "JOY_EVENT_DEBUG_CAMERA_NEXT_CAR",
    "JOY_EVENT_DEBUG_CAMERA_PREV_CAR",
    "JOY_EVENT_DEBUG_CAMERA_FOLLOW_CHOPPER",
    "JOY_EVENT_DEBUG_CAMERA_TURN_LEFT",
    "JOY_EVENT_DEBUG_CAMERA_TURN_RIGHT",
    "JOY_EVENT_DEBUG_CAMERA_TURN_DOWN",
    "JOY_EVENT_DEBUG_CAMERA_TURN_UP",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_DOWN",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_UP",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_FWD",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_BACK",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_LEFT",
    "JOY_EVENT_DEBUG_CAMERA_MOVE_RIGHT",
    "JOY_EVENT_DEBUG_CAMERA_TURBO",
    "JOY_EVENT_DEBUG_CAMERA_SUPER_TURBO",
    "JOY_EVENT_DEBUG_CAMERA_SLOW",
    "JOY_EVENT_DEBUG_CAMERA_FORWARD_BLINK",
    "JOY_EVENT_DEBUG_CAMERA_BACKWARD_BLINK",
    "JOY_EVENT_DEBUG_CAMERA_DROP_CAR",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_LEFTRIGHT",
    "JOY_EVENT_DEBUG_CAMERA_ROTATE_UPDOWN",
    "JOY_EVENT_DEBUG_CAMERA_STRAFE_LEFTRIGHT",
    "JOY_EVENT_DEBUG_CAMERA_INOUT",
    "JOY_EVENT_TRACKROUTE_EDITOR_TURN_LEFTRIGHT",
    "JOY_EVENT_TRACKROUTE_EDITOR_TURN_UPDOWN",
    "JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_FORWARDBACKWARD",
    "JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_LEFTRIGHT",
    "JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_TURBO",
    "JOY_EVENT_TRACKROUTE_EDITOR_MOVE_UP",
    "JOY_EVENT_TRACKROUTE_EDITOR_MOVE_DOWN",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_UP",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_DOWN",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_LEFT",
    "JOY_EVENT_TRACKROUTE_EDITOR_DPAD_RIGHT",
    "JOY_EVENT_TRACKROUTE_EDITOR_NEXT_ROUTE",
    "JOY_EVENT_TRACKROUTE_EDITOR_NEXT_POINT",
    "JOY_EVENT_TRACKROUTE_EDITOR_PREV_POINT",
    "JOY_EVENT_TRACKROUTE_EDITOR_RESET_POS",
    "JOY_EVENT_TRACKROUTE_EDITOR_EDIT_RIGHT_WIDTH",
    "JOY_EVENT_TRACKROUTE_EDITOR_EDIT_LEFT_WIDTH",
    "JOY_EVENT_TRACKROUTE_EDITOR_EDIT_POSITION",
    "JOY_EVENT_TOGGLE_TRACK_MARKER",
    "JOY_EVENT_CAMERA_POV_CHANGE",
    "JOY_EVENT_CAMERA_LOOKBACK",
    "JOY_EVENT_ZONE_FREEZE",
    "JOY_EVENT_ZONE_PREVIEW",
    "JOY_EVENT_ZONE_SLOMO",
    "JOY_EVENT_ZONE_WARP",
    "JOY_EVENT_REPLAY_SKIP",
    "JOY_EVENT_REPLAY_PAUSE",
    "JOY_EVENT_REPLAY_PREVIOUS_MARKER",
    "JOY_EVENT_REPLAY_NEXT_MARKER",
    "JOY_EVENT_REPLAY_PLAYBACK_SPEED",
    "JOY_EVENT_REPLAY_CAMERA_ANGLE",
    "JOY_EVENT_REPLAY_CAMERA_TARGET",
    "JOY_EVENT_REPLAY_HELP",
    "JOY_EVENT_REPLAY_HIDE_TIMELINE",
    "JOY_EVENT_REPLAY_MARK_DEMO",
    "JOY_EVENT_REPLAY_TEST_DEMO",
    "JOY_EVENT_REPLAY_ERASE_DEMO",
    "JOY_EVENT_STEER",
    "JOY_EVENT_STEER_ANALOG",
    "JOY_EVENT_THROTTLE",
    "JOY_EVENT_THROTTLE_ANALOG",
    "JOY_EVENT_THROTTLE_ANALOG_ALTERNATE",
    "JOY_EVENT_BRAKE",
    "JOY_EVENT_BRAKE_ANALOG",
    "JOY_EVENT_BRAKE_ANALOG_ALTERNATE",
    "JOY_EVENT_REVERSE",
    "JOY_EVENT_REVERSE_ANALOG",
    "JOY_EVENT_EBRAKE",
    "JOY_EVENT_EBRAKE_ANALOG",
    "JOY_EVENT_SHIFTUP",
    "JOY_EVENT_SHIFTDOWN",
    "JOY_EVENT_SHIFTUP_ALTERNATE",
    "JOY_EVENT_SHIFTDOWN_ALTERNATE",
    "JOY_EVENT_DISENGAGE_CLUTCH",
    "JOY_EVENT_DISENGAGE_CLUTCH_ANALOG",
    "JOY_EVENT_RESET_CAR",
    "JOY_EVENT_HONK_HORN",
    "JOY_EVENT_NITRO_BOOST",
    "JOY_EVENT_NITRO_BOOST_ALTERNATE",
    "JOY_EVENT_HYDRAULIC_BOUNCE",
    "JOY_EVENT_HYDRAULIC_BOUNCE_SELX",
    "JOY_EVENT_HYDRAULIC_BOUNCE_SELY",
    "JOY_EVENT_HYDRAULIC_BOUNCE_ANALOG_UD",
    "JOY_EVENT_HYDRAULIC_BOUNCE_ANALOG_LR",
    "JOY_EVENT_HYDRAULIC_PRESSURIZE",
    "JOY_EVENT_HYDRAULIC_PRESSURIZE_SELX",
    "JOY_EVENT_HYDRAULIC_PRESSURIZE_SELY",
    "JOY_EVENT_HYDRAULIC_PRESSURIZE_ANALOG_UD",
    "JOY_EVENT_HYDRAULIC_PRESSURIZE_ANALOG_LR",
    "JOY_EVENT_REQUEST_ROADBLOCK",
    "JOY_EVENT_REQUEST_HELICOPTER",
    "JOY_EVENT_FIRE_ROCKET",
    "JOY_EVENT_AIM_ROCKET_LR",
    "JOY_EVENT_AIM_ROCKET_UD",
    "JOY_EVENT_AIM_ZOOM",
    "JOY_EVENT_AUTO_PILOT",
    "Unknown_0xB6",
    "Unknown_0xB7",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_LEFT",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_RIGHT",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_LEFT_ANALOG",
    "JOY_EVENT_DRAG_RACE_CHANGE_LANE_RIGHT_ANALOG",
    "JOY_EVENT_CYCLE_HUD",
    "JOY_EVENT_PAUSE",
    "JOY_EVENT_SKIP",
    "JOY_EVENT_REQUEST_ENGAGE_INFO",
    "JOY_EVENT_REQUEST_WORLD_MAP",
    "JOY_EVENT_REQUEST_LEGEND",
    "JOY_EVENT_REQUEST_MESSAGE_SYSTEM",
    "Unknown_0xC3",
    "Unknown_0xC4",
    "Unknown_0xC5",
    "Unknown_0xC6",
    "Unknown_0xC7",
    "Unknown_0xC8",
    "Unknown_0xC9",
    "Unknown_0xCA",
    "Unknown_0xCB",
    "Unknown_0xCC",
    "Unknown_0xCD",
    "Unknown_0xCE",
    "Unknown_0xCF",
    "Unknown_0xD0",
    "Unknown_0xD1",
    "Unknown_0xD2",
    "JOY_EVENT_CAMERA_EDITOR_HOLD_LEFT",
    "JOY_EVENT_CAMERA_EDITOR_HOLD_RIGHT",
    "JOY_EVENT_CAMERA_EDITOR_UP",
    "JOY_EVENT_CAMERA_EDITOR_DOWN",
    "JOY_EVENT_CAMERA_EDITOR_LEFT",
    "JOY_EVENT_CAMERA_EDITOR_RIGHT",
    "JOY_EVENT_CAMERA_EDITOR_HELP",
    "JOY_EVENT_CAMERA_EDITOR_HIDE",
    "JOY_EVENT_CAMERA_EDITOR_SCREENSHOT",
    "JOY_EVENT_CAMERA_EDITOR_CUT",
    "JOY_EVENT_CAMERA_EDITOR_COPY",
    "JOY_EVENT_CAMERA_EDITOR_UNDO",
    "JOY_EVENT_CAMERA_EDITOR_PLAY_ANIMATION",
    "JOY_EVENT_CAMERA_EDITOR_PASTE",
    "JOY_EVENT_CAMERA_EDITOR_INSERT",
    "JOY_EVENT_CAMERA_EDITOR_SELECT",
    "JOY_EVENT_CAMERA_EDITOR_CANCEL",
    "JOY_EVENT_CAMERA_EDITOR_ALT_1",
    "JOY_EVENT_CAMERA_EDITOR_CLIP_IN",
    "JOY_EVENT_CAMERA_EDITOR_CLIP_OUT",
    "JOY_EVENT_CAMERA_EDITOR_ZOOM_IN",
    "JOY_EVENT_CAMERA_EDITOR_ZOOM_OUT",
    "JOY_EVENT_CAMERA_EDITOR_APERTURE_INC",
    "JOY_EVENT_CAMERA_EDITOR_APERTURE_DEC",
    "JOY_EVENT_CAMERA_EDITOR_LETTERBOX_IN",
    "JOY_EVENT_CAMERA_EDITOR_LETTERBOX_OUT",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_IN",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_OUT",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_UP",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_DOWN",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_TILT",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_SPIN",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_LEFT_RIGHT",
    "JOY_EVENT_CAMERA_EDITOR_BUBBLE_BACK_FORTH",
    "JOY_EVENT_CAMERA_EDITOR_SHAKE_MAG",
    "JOY_EVENT_CAMERA_EDITOR_SHAKE_FRQ",
    "JOY_EVENT_CAMERA_EDITOR_SHUTTLE_LEFT_RIGHT",
    "JOY_EVENT_CAMERA_EDITOR_SHUTTLE_SPEED"};

bool bIsEventDigitalDownOnly(unsigned int joyevent)
{
  return (joyevent == JOY_EVENT_START)
    || (joyevent == JOY_EVENT_SELECT)
    || (joyevent == JOY_EVENT_CAMERA_POV_CHANGE)
    || (joyevent == JOY_EVENT_PAUSE)
    || (joyevent == JOY_EVENT_SKIP)
    || (joyevent == JOY_EVENT_CHANGE_DEBUG_CAMERA)
    || (joyevent == JOY_EVENT_CYCLE_HUD)
    || (joyevent == JOY_EVENT_REQUEST_ENGAGE_INFO)
    || (joyevent == JOY_EVENT_REQUEST_WORLD_MAP)
    || (joyevent == JOY_EVENT_REQUEST_LEGEND)
    || (joyevent == JOY_EVENT_REQUEST_MESSAGE_SYSTEM)
    || (joyevent == JOY_EVENT_DEBUG_CAMERA_DROP_CAR);
}

bool bIsEventAnalog(unsigned int joyevent)
{
  return joyevent == JOY_EVENT_DISENGAGE_CLUTCH_ANALOG ||
    joyevent == JOY_EVENT_EBRAKE_ANALOG ||
    joyevent == JOY_EVENT_REVERSE_ANALOG ||
    joyevent == JOY_EVENT_THROTTLE_ANALOG_ALTERNATE ||
    joyevent == JOY_EVENT_BRAKE_ANALOG ||
    joyevent == JOY_EVENT_BRAKE_ANALOG_ALTERNATE ||
    joyevent == JOY_EVENT_THROTTLE_ANALOG ||
    joyevent == JOY_EVENT_THROTTLE_ANALOG_ALTERNATE ||
    joyevent == JOY_EVENT_STEER_ANALOG ||
    joyevent == JOY_EVENT_STEER ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_TURN_LEFTRIGHT ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_TURN_UPDOWN ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_FORWARDBACKWARD ||
    joyevent == JOY_EVENT_TRACKROUTE_EDITOR_DRIVE_LEFTRIGHT ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_INOUT ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_STRAFE_LEFTRIGHT ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_ROTATE_UPDOWN ||
    joyevent == JOY_EVENT_DEBUG_CAMERA_ROTATE_LEFTRIGHT ||
    joyevent == JOY_EVENT_CARSEL_ORBIT_INOUT ||
    joyevent == JOY_EVENT_CARSEL_ORBIT_UPDOWN ||
    joyevent == JOY_EVENT_CARSEL_ORBIT_LEFTRIGHT ||
    joyevent == JOY_EVENT_HYDRAULIC_BOUNCE_ANALOG_UD ||
    joyevent == JOY_EVENT_HYDRAULIC_BOUNCE_ANALOG_LR ||
    joyevent == JOY_EVENT_HYDRAULIC_PRESSURIZE_ANALOG_UD ||
    joyevent == JOY_EVENT_HYDRAULIC_PRESSURIZE_ANALOG_LR;
}
#endif  // GAME_UG2
