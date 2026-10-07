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

#ifndef XTENDEDINPUTNG_MACROS_H
#define XTENDEDINPUTNG_MACROS_H
#pragma once

#if defined(_MSC_VER)

#define MSVC_WARNINGS_OFF                      \
  __pragma(warning(push, 0)) __pragma(warning( \
      disable : 4244 4265 4267 4350 4472 4509 4548 4623 4710 4985 6001 6011 6255 6285 6387 6031 6320 6258 4755 4625 4626 4702 26812 26439 26451 26495 26819 26498))
#define MSVC_WARNINGS_RESTORE __pragma(warning(pop))

#define MSVC_RUNTIME_CHECKS_OFF     __pragma(runtime_checks("", off)) __pragma(check_stack(off)) __pragma(strict_gs_check(push, off))
#define MSVC_RUNTIME_CHECKS_RESTORE __pragma(runtime_checks("", restore)) __pragma(check_stack()) __pragma(strict_gs_check(pop))

#else  // defined(_MSC_VER)

#define MSVC_WARNINGS_OFF
#define MSVC_WARNINGS_RESTORE
#define MSVC_RUNTIME_CHECKS_OFF
#define MSVC_RUNTIME_CHECKS_RESTORE

#endif  // defined(_MSC_VER)

#endif