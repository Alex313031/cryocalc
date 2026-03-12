#ifndef CRYOCALC_STRESS_CPU_H_
#define CRYOCALC_STRESS_CPU_H_

#include <winternl.h>

#include <atomic>

#include "../framework.h"

/* Typedefs for accessing system .dll functions through GetProcAddress() */

typedef void (WINAPI* GetNativeSystemInfo_t)(SYSTEM_INFO* lpSystemInfo);

typedef NTSTATUS (WINAPI* NtQuerySystemInformation_t)(SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                                      PVOID SystemInformation,
                                                      ULONG SystemInformationLength,
                                                      PULONG ReturnLength);

typedef BOOL (WINAPI* GetSystemTimes_t)(LPFILETIME lpIdleTime,
                                        LPFILETIME lpKernelTime,
                                        LPFILETIME lpUserTime);

// Runs GetCPUPercentImpl, and verifies that it is between 0 and 100
const float GetCPUPercent();

// Gets the number of logical CPU threads of the host system.
DWORD GetLogicalProcessorCount();

#endif // CRYOCALC_STRESS_CPU_H_
