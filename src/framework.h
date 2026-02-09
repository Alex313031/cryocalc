// framwork.h: include file for standard system include files like windows.h,
// or project wide specific include files, such as C or C++ standard library
// headers. It can be precompiled, in which case, add a "framework.cc" file.

#ifndef CRYOCALC_FRAMEWORK_H_
#define CRYOCALC_FRAMEWORK_H_

#ifdef __cplusplus
 #if __cplusplus < 201103L
  #error CryoCalc only supports C++11 and above
 #endif
#endif

#include "./version.h"

// We need to define _UNICODE and UNICODE for TCHAR
#ifndef UNICODE
 #define UNICODE
#endif

#ifndef _UNICODE
 #define _UNICODE
#endif

#if  defined(__clang__) && defined(_UNICODE)
 #pragma code_page(65001) // UTF-8
#endif // __clang__

#ifndef __FUNC__
 #define __FUNC__ __func__
#endif

#ifndef __MINGW32__
 #include <WinSDKVer.h> // Doesn't exist in MinGW
#endif // __MINGW32__

#ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x0500 // Windows 2000
#endif // _WIN32_WINNT
#ifndef WINVER
 #define WINVER 0x0500 // Same as above
#endif // WINVER
#ifndef _WIN64_WINNT
 #define _WIN64_WINNT 0x0502 // Minimum version for 64 bit, Windows Server 2003
#endif // _WIN64_WINNT
#ifndef _WIN32_IE
 #define _WIN32_IE 0x0501 // Minimum Internet Explorer version for common controls
#endif // _WIN32_IE

#if _WIN32_WINNT <= 0x0600 // If we are less than Windows 7, use old ATL for safety
 #ifndef _ATL_XP_TARGETING
  #define _ATL_XP_TARGETING // For using XP-compatible ATL/MFC functions
 #endif // _ATL_XP_TARGETING
#endif // _WIN32_WINNT <= 0x0600

#ifndef __MINGW32__
 #include <SDKDDKVer.h> // Doesn't exist in MinGW
#endif // __MINGW32__

// Windows Header Files
#include <windows.h> // Main Windows header
#include <commctrl.h> // Common controls header
#include <mmsystem.h> // For playing sounds
#include <shlwapi.h> // For DLL functions

// C Runtime Headers
#include <tchar.h> // For TCHAR, and automatically deducing wchar_t type

// C++ STL Headers
#include <iomanip> // std::setprecision
#include <iostream> // std::cout std::wcout
#include <sstream> // std::ostringstream std::wostringstream
#include <string> // std::string std::wstring
#include <vector> // std::vector
#include <thread> // For threading support

inline constexpr bool is_dcheck =
#ifdef DCHECK
 true;
#else
 false;
#endif // DCHECK

#endif // CRYOCALC_FRAMEWORK_H_
