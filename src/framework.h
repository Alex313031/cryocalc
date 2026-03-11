// framwork.h: include file for standard system include files like windows.h,
// or project wide specific include files, such as C or C++ standard library
// headers. It can be precompiled, in which case, add an "framework.cc" file.

#ifndef CRYOCALC_FRAMEWORK_H_
#define CRYOCALC_FRAMEWORK_H_

#ifdef __cplusplus
 #if __cplusplus < 201103L
  #error CryoCalc only supports C++11 and above
 #endif
#endif

// Defines target Windows version and app version constants
#include "./version.h"

#if defined(__clang__) && defined(_UNICODE)
 #pragma code_page(65001) // UTF-8
#endif

#ifndef __FUNC__
 #define __FUNC__ __func__
#endif

// Windows Header Files

// On MSVC, <windows.h> defines a small subset of NTSTATUS codes, which then
// conflict with the full set in <ntstatus.h>. WIN32_NO_STATUS suppresses those
// definitions so <ntstatus.h> can own them without redefinition errors.
// MinGW handles this automatically, so the guard is MSVC-only.
#ifdef _MSC_VER
# define WIN32_NO_STATUS
#endif
#include <windows.h>  // Main Windows header
#ifdef _MSC_VER
# undef WIN32_NO_STATUS
#endif
#include <ntstatus.h> // Full NTSTATUS codes (e.g. STATUS_SUCCESS)

#include <commctrl.h> // Common controls header
#include <mmsystem.h> // For playing sounds
#include <shlwapi.h>  // For DLL functions

// C Runtime Headers
#include <tchar.h> // For TCHAR, and automatically deducing wchar_t type

// C++ STL Headers
#include <algorithm> // std::clamp
#include <iomanip>   // std::setprecision
#include <iostream>  // std::cout/std::wcout
#include <sstream>   // std::ostringstream/std::wostringstream
#include <string>    // std::string/std::wstring
#include <thread>    // For threading support
#include <vector>    // std::vector

inline constexpr bool is_dcheck =
#ifdef DCHECK
    true;
#else
    false;
#endif // DCHECK

#endif // CRYOCALC_FRAMEWORK_H_
