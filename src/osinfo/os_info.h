#ifndef OSINFO_DLL_OS_INFO_H_
#define OSINFO_DLL_OS_INFO_H_

#include "export.h"

extern "C"{

DLL_EXPORT unsigned long long __cdecl GetOsInfo(unsigned long spoofedInfo);

}

#endif // OSINFO_DLL_OS_INFO_H_
