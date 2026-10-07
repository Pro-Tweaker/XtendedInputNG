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
#include "fe/Buttons.h"

int GetAppropriateButtonTex(WORD buttonmask) {
  switch (ControllerIconMode) {
    case CONTROLLERICON_PS3:
      switch (buttonmask) {
        case XINPUT_GAMEPAD_DPAD_UP:
          return PS3_DPAD_UP_HASH;
        case XINPUT_GAMEPAD_DPAD_DOWN:
          return PS3_DPAD_DOWN_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFT:
          return PS3_DPAD_LEFT_HASH;
        case XINPUT_GAMEPAD_DPAD_RIGHT:
          return PS3_DPAD_RIGHT_HASH;
        case XINPUT_GAMEPAD_START:
          return PS3_START_HASH;
        case XINPUT_GAMEPAD_BACK:
          return PS3_SELECT_HASH;
        case XINPUT_GAMEPAD_LEFT_THUMB:
          return PS3_L3_HASH;
        case XINPUT_GAMEPAD_RIGHT_THUMB:
          return PS3_R3_HASH;
        case XINPUT_GAMEPAD_LEFT_SHOULDER:
          return PS3_L1_HASH;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER:
          return PS3_R1_HASH;
        case XINPUT_GAMEPAD_A:
          return PS3_CROSS_HASH;
        case XINPUT_GAMEPAD_B:
          return PS3_CIRCLE_HASH;
        case XINPUT_GAMEPAD_X:
          return PS3_SQUARE_HASH;
        case XINPUT_GAMEPAD_Y:
          return PS3_TRIANGLE_HASH;
        case XINPUT_GAMEPAD_LT_CONFIGDEF:
          return PS3_L2_HASH;
        case XINPUT_GAMEPAD_RT_CONFIGDEF:
          return PS3_R2_HASH;
        case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
          return PS3_LANALOG_HASH;
        case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
          return PS3_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
          return PS3_RANALOG_HASH;
        case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
          return PS3_RANALOG_HASH;
        case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
          return PS3_LANALOG_UP_HASH;
        case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
          return PS3_LANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
          return PS3_LANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
          return PS3_LANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
          return PS3_RANALOG_UP_HASH;
        case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
          return PS3_RANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
          return PS3_RANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
          return PS3_RANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_DPAD_CONFIGDEF:
          return PS3_DPAD_HASH;
        case XINPUT_GAMEPAD_DPAD_UPDOWN_CONFIGDEF:
          return PS3_DPAD_Y_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFTRIGHT_CONFIGDEF:
          return PS3_DPAD_X_HASH;
        case XINPUT_GAMEPAD_LS_CONFIGDEF:
          return PS3_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_CONFIGDEF:
          return PS3_RANALOG_HASH;

        default:
          break;
      }
    case CONTROLLERICON_XBOX360:
      switch (buttonmask) {
        case XINPUT_GAMEPAD_DPAD_UP:
          return XB360_DPAD_UP_HASH;
        case XINPUT_GAMEPAD_DPAD_DOWN:
          return XB360_DPAD_DOWN_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFT:
          return XB360_DPAD_LEFT_HASH;
        case XINPUT_GAMEPAD_DPAD_RIGHT:
          return XB360_DPAD_RIGHT_HASH;
        case XINPUT_GAMEPAD_START:
          return XB360_START_HASH;
        case XINPUT_GAMEPAD_BACK:
          return XB360_BACK_HASH;
        case XINPUT_GAMEPAD_LEFT_THUMB:
          return XB360_LS_HASH;
        case XINPUT_GAMEPAD_RIGHT_THUMB:
          return XB360_RS_HASH;
        case XINPUT_GAMEPAD_LEFT_SHOULDER:
          return XB360_LB_HASH;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER:
          return XB360_RB_HASH;
        case XINPUT_GAMEPAD_A:
          return XB360_A_HASH;
        case XINPUT_GAMEPAD_B:
          return XB360_B_HASH;
        case XINPUT_GAMEPAD_X:
          return XB360_X_HASH;
        case XINPUT_GAMEPAD_Y:
          return XB360_Y_HASH;
        case XINPUT_GAMEPAD_LT_CONFIGDEF:
          return XB360_LT_HASH;
        case XINPUT_GAMEPAD_RT_CONFIGDEF:
          return XB360_RT_HASH;
        case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
          return XB360_LANALOG_HASH;
        case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
          return XB360_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
          return XB360_RANALOG_HASH;
        case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
          return XB360_RANALOG_HASH;
        case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
          return XB360_LANALOG_UP_HASH;
        case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
          return XB360_LANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
          return XB360_LANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
          return XB360_LANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
          return XB360_RANALOG_UP_HASH;
        case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
          return XB360_RANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
          return XB360_RANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
          return XB360_RANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_DPAD_CONFIGDEF:
          return XB360_DPAD_HASH;
        case XINPUT_GAMEPAD_DPAD_UPDOWN_CONFIGDEF:
          return XB360_DPAD_Y_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFTRIGHT_CONFIGDEF:
          return XB360_DPAD_X_HASH;
        case XINPUT_GAMEPAD_LS_CONFIGDEF:
          return XB360_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_CONFIGDEF:
          return XB360_RANALOG_HASH;

        default:
          break;
      }
    case CONTROLLERICON_PS4:
      switch (buttonmask) {
        case XINPUT_GAMEPAD_DPAD_UP:
          return PS4_DPAD_UP_HASH;
        case XINPUT_GAMEPAD_DPAD_DOWN:
          return PS4_DPAD_DOWN_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFT:
          return PS4_DPAD_LEFT_HASH;
        case XINPUT_GAMEPAD_DPAD_RIGHT:
          return PS4_DPAD_RIGHT_HASH;
        case XINPUT_GAMEPAD_START:
          return PS4_START_HASH;
        case XINPUT_GAMEPAD_BACK:
          return PS4_SHARE_HASH;
        case XINPUT_GAMEPAD_LEFT_THUMB:
          return PS4_L3_HASH;
        case XINPUT_GAMEPAD_RIGHT_THUMB:
          return PS4_R3_HASH;
        case XINPUT_GAMEPAD_LEFT_SHOULDER:
          return PS4_L1_HASH;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER:
          return PS4_R1_HASH;
        case XINPUT_GAMEPAD_A:
          return PS4_CROSS_HASH;
        case XINPUT_GAMEPAD_B:
          return PS4_CIRCLE_HASH;
        case XINPUT_GAMEPAD_X:
          return PS4_SQUARE_HASH;
        case XINPUT_GAMEPAD_Y:
          return PS4_TRIANGLE_HASH;
        case XINPUT_GAMEPAD_LT_CONFIGDEF:
          return PS4_L2_HASH;
        case XINPUT_GAMEPAD_RT_CONFIGDEF:
          return PS4_R2_HASH;
        case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
          return PS4_LANALOG_X_HASH;
        case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
          return PS4_LANALOG_Y_HASH;
        case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
          return PS4_RANALOG_X_HASH;
        case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
          return PS4_RANALOG_Y_HASH;
        case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
          return PS4_LANALOG_UP_HASH;
        case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
          return PS4_LANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
          return PS4_LANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
          return PS4_LANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
          return PS4_RANALOG_UP_HASH;
        case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
          return PS4_RANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
          return PS4_RANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
          return PS4_RANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_DPAD_CONFIGDEF:
          return PS4_DPAD_HASH;
        case XINPUT_GAMEPAD_DPAD_UPDOWN_CONFIGDEF:
          return PS4_DPAD_Y_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFTRIGHT_CONFIGDEF:
          return PS4_DPAD_X_HASH;
        case XINPUT_GAMEPAD_LS_CONFIGDEF:
          return PS4_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_CONFIGDEF:
          return PS4_RANALOG_HASH;

        default:
          break;
      }
    case CONTROLLERICON_SWITCH:
      switch (buttonmask) {
        case XINPUT_GAMEPAD_DPAD_UP:
          return SWITCH_DPAD_UP_HASH;
        case XINPUT_GAMEPAD_DPAD_DOWN:
          return SWITCH_DPAD_DOWN_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFT:
          return SWITCH_DPAD_LEFT_HASH;
        case XINPUT_GAMEPAD_DPAD_RIGHT:
          return SWITCH_DPAD_RIGHT_HASH;
        case XINPUT_GAMEPAD_START:
          return SWITCH_PLUS_HASH;
        case XINPUT_GAMEPAD_BACK:
          return SWITCH_MINUS_HASH;
        case XINPUT_GAMEPAD_LEFT_THUMB:
          return SWITCH_LS_HASH;
        case XINPUT_GAMEPAD_RIGHT_THUMB:
          return SWITCH_RS_HASH;
        case XINPUT_GAMEPAD_LEFT_SHOULDER:
          return SWITCH_L_HASH;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER:
          return SWITCH_R_HASH;
        case XINPUT_GAMEPAD_A:
          return SWITCH_B_HASH;
        case XINPUT_GAMEPAD_B:
          return SWITCH_A_HASH;
        case XINPUT_GAMEPAD_X:
          return SWITCH_Y_HASH;
        case XINPUT_GAMEPAD_Y:
          return SWITCH_X_HASH;
        case XINPUT_GAMEPAD_LT_CONFIGDEF:
          return SWITCH_ZL_HASH;
        case XINPUT_GAMEPAD_RT_CONFIGDEF:
          return SWITCH_ZR_HASH;
        case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
          return SWITCH_LANALOG_HASH;
        case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
          return SWITCH_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
          return SWITCH_RANALOG_HASH;
        case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
          return SWITCH_RANALOG_HASH;
        case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
          return SWITCH_LANALOG_UP_HASH;
        case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
          return SWITCH_LANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
          return SWITCH_LANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
          return SWITCH_LANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
          return SWITCH_RANALOG_UP_HASH;
        case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
          return SWITCH_RANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
          return SWITCH_RANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
          return SWITCH_RANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_DPAD_CONFIGDEF:
          return SWITCH_DPAD_HASH;
        case XINPUT_GAMEPAD_DPAD_UPDOWN_CONFIGDEF:
          return SWITCH_DPAD_Y_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFTRIGHT_CONFIGDEF:
          return SWITCH_DPAD_X_HASH;
        case XINPUT_GAMEPAD_LS_CONFIGDEF:
          return SWITCH_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_CONFIGDEF:
          return SWITCH_RANALOG_HASH;

        default:
          break;
      }
    case CONTROLLERICON_XB360MW:
      switch (buttonmask) {
        case XINPUT_GAMEPAD_DPAD_UP:
          return XB360MW_DPAD_UP_HASH;
        case XINPUT_GAMEPAD_DPAD_DOWN:
          return XB360MW_DPAD_DOWN_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFT:
          return XB360MW_DPAD_LEFT_HASH;
        case XINPUT_GAMEPAD_DPAD_RIGHT:
          return XB360MW_DPAD_RIGHT_HASH;
        case XINPUT_GAMEPAD_START:
          return XB360MW_START_HASH;
        case XINPUT_GAMEPAD_BACK:
          return XB360MW_BACK_HASH;
        case XINPUT_GAMEPAD_LEFT_THUMB:
          return XB360MW_LS_HASH;
        case XINPUT_GAMEPAD_RIGHT_THUMB:
          return XB360MW_RS_HASH;
        case XINPUT_GAMEPAD_LEFT_SHOULDER:
          return XB360MW_LB_HASH;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER:
          return XB360MW_RB_HASH;
        case XINPUT_GAMEPAD_A:
          return XB360MW_A_HASH;
        case XINPUT_GAMEPAD_B:
          return XB360MW_B_HASH;
        case XINPUT_GAMEPAD_X:
          return XB360MW_X_HASH;
        case XINPUT_GAMEPAD_Y:
          return XB360MW_Y_HASH;
        case XINPUT_GAMEPAD_LT_CONFIGDEF:
          return XB360MW_LT_HASH;
        case XINPUT_GAMEPAD_RT_CONFIGDEF:
          return XB360MW_RT_HASH;
        case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
          return XB360MW_LANALOG_HASH;
        case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
          return XB360MW_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
          return XB360MW_RANALOG_HASH;
        case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
          return XB360MW_RANALOG_HASH;
        case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
          return XB360MW_LANALOG_UP_HASH;
        case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
          return XB360MW_LANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
          return XB360MW_LANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
          return XB360MW_LANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
          return XB360MW_RANALOG_UP_HASH;
        case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
          return XB360MW_RANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
          return XB360MW_RANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
          return XB360MW_RANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_DPAD_CONFIGDEF:
          return XB360MW_DPAD_HASH;
        case XINPUT_GAMEPAD_DPAD_UPDOWN_CONFIGDEF:
          return XB360MW_DPAD_Y_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFTRIGHT_CONFIGDEF:
          return XB360MW_DPAD_X_HASH;
        case XINPUT_GAMEPAD_LS_CONFIGDEF:
          return XB360MW_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_CONFIGDEF:
          return XB360MW_RANALOG_HASH;

        default:
          break;
      }
    case CONTROLLERICON_XBOXONE:
    default:
      switch (buttonmask) {
        case XINPUT_GAMEPAD_DPAD_UP:
          return XBONE_DPAD_UP_HASH;
        case XINPUT_GAMEPAD_DPAD_DOWN:
          return XBONE_DPAD_DOWN_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFT:
          return XBONE_DPAD_LEFT_HASH;
        case XINPUT_GAMEPAD_DPAD_RIGHT:
          return XBONE_DPAD_RIGHT_HASH;
        case XINPUT_GAMEPAD_START:
          return XBONE_MENU_HASH;
        case XINPUT_GAMEPAD_BACK:
          return XBONE_VIEW_HASH;
        case XINPUT_GAMEPAD_LEFT_THUMB:
          return XBONE_LS_HASH;
        case XINPUT_GAMEPAD_RIGHT_THUMB:
          return XBONE_RS_HASH;
        case XINPUT_GAMEPAD_LEFT_SHOULDER:
          return XBONE_LB_HASH;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER:
          return XBONE_RB_HASH;
        case XINPUT_GAMEPAD_A:
          return XBONE_A_HASH;
        case XINPUT_GAMEPAD_B:
          return XBONE_B_HASH;
        case XINPUT_GAMEPAD_X:
          return XBONE_X_HASH;
        case XINPUT_GAMEPAD_Y:
          return XBONE_Y_HASH;
        case XINPUT_GAMEPAD_LT_CONFIGDEF:
          return XBONE_LT_HASH;
        case XINPUT_GAMEPAD_RT_CONFIGDEF:
          return XBONE_RT_HASH;
        case XINPUT_GAMEPAD_LS_X_CONFIGDEF:
          return XBONE_LANALOG_X_HASH;
        case XINPUT_GAMEPAD_LS_Y_CONFIGDEF:
          return XBONE_LANALOG_Y_HASH;
        case XINPUT_GAMEPAD_RS_X_CONFIGDEF:
          return XBONE_RANALOG_X_HASH;
        case XINPUT_GAMEPAD_RS_Y_CONFIGDEF:
          return XBONE_RANALOG_Y_HASH;
        case XINPUT_GAMEPAD_LS_UP_CONFIGDEF:
          return XBONE_LANALOG_UP_HASH;
        case XINPUT_GAMEPAD_LS_DOWN_CONFIGDEF:
          return XBONE_LANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_LS_LEFT_CONFIGDEF:
          return XBONE_LANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_LS_RIGHT_CONFIGDEF:
          return XBONE_LANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_RS_UP_CONFIGDEF:
          return XBONE_RANALOG_UP_HASH;
        case XINPUT_GAMEPAD_RS_DOWN_CONFIGDEF:
          return XBONE_RANALOG_DOWN_HASH;
        case XINPUT_GAMEPAD_RS_LEFT_CONFIGDEF:
          return XBONE_RANALOG_LEFT_HASH;
        case XINPUT_GAMEPAD_RS_RIGHT_CONFIGDEF:
          return XBONE_RANALOG_RIGHT_HASH;
        case XINPUT_GAMEPAD_DPAD_CONFIGDEF:
          return XBONE_DPAD_HASH;
        case XINPUT_GAMEPAD_DPAD_UPDOWN_CONFIGDEF:
          return XBONE_DPAD_Y_HASH;
        case XINPUT_GAMEPAD_DPAD_LEFTRIGHT_CONFIGDEF:
          return XBONE_DPAD_X_HASH;
        case XINPUT_GAMEPAD_LS_CONFIGDEF:
          return XBONE_LANALOG_HASH;
        case XINPUT_GAMEPAD_RS_CONFIGDEF:
          return XBONE_RANALOG_HASH;

        default:
          break;
      }
      break;
  }
  return 0;
}
