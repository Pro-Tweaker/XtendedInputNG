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
#include "plugin/PatternScan.h"

namespace {

bool ParsePattern(const char* text, std::vector<int>& out) {
  out.clear();
  const char* p = text;
  while (*p) {
    while (*p == ' ') p++;
    if (!*p) break;
    if (*p == '?') {
      out.push_back(-1);
      while (*p && *p != ' ') p++;
    } else {
      char* end = NULL;
      long  v   = strtol(p, &end, 16);
      if (end == p || v < 0 || v > 255) return false;
      out.push_back((int)v);
      p = end;
    }
  }
  return !out.empty();
}

}  // namespace

uint8_t* FindPattern(const char* text) {
  std::vector<int> pat;
  if (!ParsePattern(text, pat)) return NULL;

  auto base = (uint8_t*)GetModuleHandleW(NULL);
  auto dos  = (IMAGE_DOS_HEADER*)base;
  auto nt   = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
  auto sec  = IMAGE_FIRST_SECTION(nt);
  for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, sec++) {
    if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;
    uint8_t* start = base + sec->VirtualAddress;
    size_t   size  = sec->Misc.VirtualSize;
    if (size < pat.size()) continue;
    for (size_t off = 0; off + pat.size() <= size; off++) {
      size_t k = 0;
      while (k < pat.size() && (pat[k] < 0 || start[off + k] == (uint8_t)pat[k])) k++;
      if (k == pat.size()) return start + off;
    }
  }
  return NULL;
}
