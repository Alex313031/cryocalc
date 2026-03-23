#ifndef CRYOCALC_STRESS_CPU_H_
#define CRYOCALC_STRESS_CPU_H_

#include <winternl.h>

#include "common.h"

/* Typedefs for accessing system .dll functions through GetProcAddress() */
typedef void (WINAPI* GetNativeSystemInfo_t)(SYSTEM_INFO* lpSystemInfo);

typedef NTSTATUS (WINAPI* NtQuerySystemInformation_t)(SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                                      PVOID SystemInformation,
                                                      ULONG SystemInformationLength,
                                                      PULONG ReturnLength);

typedef BOOL (WINAPI* GetSystemTimes_t)(LPFILETIME lpIdleTime,
                                        LPFILETIME lpKernelTime,
                                        LPFILETIME lpUserTime);

#ifndef NT_SUCCESS
 #define NT_SUCCESS(s) (((LONG)(s)) >= 0)
#endif

// Use a local struct name to avoid any conflict with <winternl.h>.
struct SysProcPerfInfo {
  LARGE_INTEGER IdleTime;
  LARGE_INTEGER KernelTime; // includes IdleTime
  LARGE_INTEGER UserTime;
  LARGE_INTEGER DpcTime;
  LARGE_INTEGER InterruptTime;
  ULONG         InterruptCount;
};

// Timer stuff
static constexpr UINT kUpdateTimerId = 1u;

// Graph Update speed intervals in milliseconds
static constexpr UINT kSpeedLow  = 2000u;
static constexpr UINT kSpeedMed  = 1000u;
static constexpr UINT kSpeedHigh =  500u;

// Calls UpdatePerfData() to take a fresh sample, then returns g_snapshot.cpu_percent
// as a float clamped to [0.0, 100.0].
const float GetCPUPercent();

// Gets the number of logical CPU threads of the host system.
DWORD GetLogicalProcessorCount();

// Returns true if InitPerfData has been run at least once
const bool IsPerfDataInitialized();

// Dynamically load NtQuerySystemInformation from ntdll.dll and take an
// initial CPU counter sample so the first timer tick gives a real reading.
bool InitPerfData();

// Re-sample all counters and update the internal PerfSnapshot.
// Call this once per timer tick before requesting the snapshot.
void UpdatePerfData();

// Returns a const reference to the last snapshot produced by UpdatePerfData().
const PerfSnapshot& GetPerfSnapshot();

// Release any resources from InitPerfData().
void CleanupPerfData();

#endif // CRYOCALC_STRESS_CPU_H_
