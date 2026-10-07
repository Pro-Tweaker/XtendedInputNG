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
#include "plugin/Util.h"

int bStringHash(char* str) {
  if (str == NULL) return 0;

  char* _str   = ((char*)str);
  int   result = -1;

  while (*_str != 0) {
    result = result * 0x21 + (unsigned int)(*_str);
    _str   = _str + 1;
  }

  return result;
}

void FixWorkingDirectory() {
  wchar_t ExecutablePath[MAX_PATH];

  DWORD len = GetModuleFileNameW(NULL, ExecutablePath, MAX_PATH);
  if (len == 0 || len >= MAX_PATH) return;  // failed or truncated, leave the working directory alone
  if (PathRemoveFileSpecW(ExecutablePath)) SetCurrentDirectoryW(ExecutablePath);
}
