#ifndef OSINFO_DLL_OS_INFO_H_
#define OSINFO_DLL_OS_INFO_H_

#include "os_info_dll.h"

extern "C"{

unsigned long WinVer;

namespace {
  static bool is_win11;

  static ULONG NT_MAJOR;

  static ULONG NT_MINOR;

  static ULONG NT_BUILD;

  static std::wstring NT_CSD_VERSION;

  static std::string NT_SERVICE_PACK;

  static USHORT NT_SUITE;

  static UCHAR NT_TYPE;
}

static std::string NT_FEATURE_VERSION;

static std::string NT_POST_STRING;

float __cdecl concatToFloat(int major, int minor);

unsigned long __cdecl combineToHex(unsigned long high, unsigned long low);

static std::string const __cdecl GetNTString();

static std::wstring __cdecl StringToWstring(const std::string& str);

static std::string __cdecl WstringToString(const std::wstring& wstr);

inline void __cdecl NotReachedImpl(std::string func_name);

#ifndef NOTREACHED
#define NOTREACHED() \
        std::string func_name(__FUNC__); \
        NotReachedImpl(func_name);
#endif // NOTREACHED

}

#endif // OSINFO_DLL_OS_INFO_H_
