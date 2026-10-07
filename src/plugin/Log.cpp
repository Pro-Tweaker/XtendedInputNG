/*
// clang-format off
//
//    Xtended Input NG - native gamepad support for the PC versions of Need for Speed games
//    Continues xan1242's NFS-XtendedInput and NFSU-XtendedInput
//
//    MIT License
//
//    Copyright (c) 2026 Pro-Tweaker
//
// clang-format on
*/

#include "stdafx.h"
#include "plugin/Log.h"
#include "plugin/Plugin.h"
#include <cstdarg>

static FILE* g_LogFile = NULL;

void LogOpen() {
  if (g_LogFile) return;
  g_LogFile = fopen("scripts\\XtendedInputNG.log", "w");
  if (!g_LogFile) return;
  Log("XtendedInputNG (%s) built %s %s", GAME_NAME_STR, __DATE__, __TIME__);
}

void Log(const char* fmt, ...) {
  if (!g_LogFile) return;
  SYSTEMTIME t;
  GetLocalTime(&t);
  fprintf(g_LogFile, "[%02d:%02d:%02d.%03d] ", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
  va_list args;
  va_start(args, fmt);
  vfprintf(g_LogFile, fmt, args);
  va_end(args);
  fputc('\n', g_LogFile);
  fflush(g_LogFile);
}

// true when the game's folder is inside a location UAC virtualizes (Program Files, Windows)
static bool InVirtualizedFolder() {
  wchar_t exe[MAX_PATH];
  DWORD   len = GetModuleFileNameW(NULL, exe, MAX_PATH);
  if (len == 0 || len >= MAX_PATH) return false;

  const wchar_t* vars[] = {L"ProgramFiles", L"ProgramFiles(x86)", L"ProgramW6432", L"SystemRoot"};
  for (const wchar_t* var : vars) {
    wchar_t root[MAX_PATH];
    DWORD   n = GetEnvironmentVariableW(var, root, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) continue;
    if (_wcsnicmp(exe, root, n) == 0 && (exe[n] == L'\\' || exe[n] == L'/')) return true;
  }
  return false;
}

void LogVirtualizationWarning() {
  HANDLE token = NULL;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return;
  DWORD enabled = 0, len = 0;
  const bool ok = GetTokenInformation(token, TokenVirtualizationEnabled, &enabled, sizeof(enabled), &len) != 0;
  CloseHandle(token);
  // virtualization is on by default for 32-bit processes, it only bites inside protected folders
  if (ok && enabled && InVirtualizedFolder()) {
    Log("WARNING: the game is installed in a UAC-virtualized folder (Program Files/Windows) and file virtualization is active. "
        "Changes made to scripts\\*.ini with admin rights may not be the files the game reads. "
        "Run the game as administrator or install it outside Program Files.");
  }
}
