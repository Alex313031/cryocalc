#include "stdafx.h"
#include "os_info.h"

DLL_EXPORT unsigned long long __cdecl GetOsInfo(unsigned long spoofedInfo) {
  if (spoofedInfo > 0x0L) {
    return static_cast<unsigned long long>(spoofedInfo);
  }
  return 0x0601LL;
}
