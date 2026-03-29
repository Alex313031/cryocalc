#ifndef CRYOCALC_STRESS_CPU_H_
#define CRYOCALC_STRESS_CPU_H_

#include <winternl.h>

#include "common.h"

/* Typedefs for accessing system .dll functions through GetProcAddress() */
typedef void(WINAPI* GetNativeSystemInfo_t)(SYSTEM_INFO* lpSystemInfo);

typedef NTSTATUS(WINAPI* NtQuerySystemInformation_t)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

typedef BOOL(WINAPI* GetSystemTimes_t)(LPFILETIME lpIdleTime,
                                       LPFILETIME lpKernelTime,
                                       LPFILETIME lpUserTime);

extern bool g_first_cpu_sample;

extern NtQuerySystemInformation_t g_NtQSI;

extern GetSystemTimes_t g_GetSystemTimes;

extern int g_num_cpus;

// Use a local struct name to avoid any conflict with <winternl.h>.
struct SysProcPerfInfo {
  LARGE_INTEGER IdleTime;
  LARGE_INTEGER KernelTime; // includes IdleTime
  LARGE_INTEGER UserTime;
  LARGE_INTEGER DpcTime;
  LARGE_INTEGER InterruptTime;
  ULONG InterruptCount;
};

// Timer stuff
static constexpr UINT kUpdateTimerId = 1u;

// Graph Update speed intervals in milliseconds
static constexpr UINT kSpeedLow  = 2000u;
static constexpr UINT kSpeedMed  = 1000u;
static constexpr UINT kSpeedHigh = 500u;

// Calls UpdateCPUPerfData() to take a fresh sample, then returns g_snapshot.cpu_percent
// as a float clamped to [0.0, 100.0].
const float GetCPUPercent();

// Gets the number of logical CPU threads of the host system.
DWORD GetLogicalProcessorCount();

// Re-sample counters and update the internal PerfSnapshot cpu_percent.
// Call this once per timer tick before requesting the snapshot.
void UpdateCPUPerfData();

#endif // CRYOCALC_STRESS_CPU_H_
