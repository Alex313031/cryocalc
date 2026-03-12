#include "cpu.h"

#include <os_info_dll.h>

#include <chrono>

#include "reporting.h"

struct SysProcPerfInfo {
  LARGE_INTEGER IdleTime;
  LARGE_INTEGER KernelTime; // includes IdleTime
  LARGE_INTEGER UserTime;
  LARGE_INTEGER DpcTime;
  LARGE_INTEGER InterruptTime;
  ULONG         InterruptCount;
};

static GetNativeSystemInfo_t pfnGetNativeSystemInfo = nullptr;

// Gets the current total CPU usage % and is supposed
// to return it as a float between 0.0 and 100.0.
// The ONLY caller of this function should be GetCPUPercent.
static float GetCPUPercentImpl() {
  // Helper: pack a FILETIME into a ULONGLONG (100ns intervals)
  auto FileTimeToULL = [](const FILETIME& ft) -> ULONGLONG {
    return (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
  };

  // State preserved between calls for delta computation.
  // Only MonitorCPU() calls this (a single thread), so plain static is safe.
  static bool first_call   = true;
  static ULONGLONG prev_idle   = 0;
  static ULONGLONG prev_kernel = 0;
  static ULONGLONG prev_user   = 0;

  // GetSystemTimes is only Windows XP SP1+, so it is dynamically loaded
  // so that it gracefully falls back to NtQuerySystemInformation on Windows 2000.
  static GetSystemTimes_t pfnGetSystemTimes = reinterpret_cast<GetSystemTimes_t>(
      GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetSystemTimes"));

  // Log the chosen backend function only once on the first call.
  static bool backend_logged = false;
  static const bool legacy_fallback = IsWinOlderThan(kWinXP);
  if (pfnGetSystemTimes && !legacy_fallback) {
    FILETIME idle_ft, kernel_ft, user_ft;
    if (!pfnGetSystemTimes(&idle_ft, &kernel_ft, &user_ft)) {
      return 0.0f;
    }
    if (!backend_logged && debug_mode) {
      backend_logged = true;
      LOG(DEBUG) << L"CPU monitoring using GetSystemTimes() (Windows XP SP1+).";
    }
    const ULONGLONG idle   = FileTimeToULL(idle_ft);
    const ULONGLONG kernel = FileTimeToULL(kernel_ft); // includes idle
    const ULONGLONG user   = FileTimeToULL(user_ft);

    const ULONGLONG idle_delta   = idle   - prev_idle;
    const ULONGLONG kernel_delta = kernel - prev_kernel;
    const ULONGLONG user_delta   = user   - prev_user;

    prev_idle   = idle;
    prev_kernel = kernel;
    prev_user   = user;

    // First call has no previous sample; delta would be meaningless.
    if (first_call) {
      first_call = false;
      return 0.0f;
    }

    const ULONGLONG total = kernel_delta + user_delta;
    if (total == 0) {
      return 0.0f;
    }
    // kernel includes idle, so busy_time = (kernel - idle) + user = total - idle
    const ULONGLONG busy_time = total - idle_delta;
    return std::clamp(
        static_cast<float>(busy_time) / static_cast<float>(total) * 100.0f,
        0.0f, 100.0f);
  } else {
    // Fallback for Windows 2000
    // KernelTime includes IdleTime, same relationship as above.
    static NtQuerySystemInformation_t pfnNtQuery =
        reinterpret_cast<NtQuerySystemInformation_t>(
            GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation"));
    if (!pfnNtQuery) {
      return 0.0f; // Should never happen
    }
    if (!backend_logged && debug_mode) {
      backend_logged = true;
      LOG(DEBUG) << L"CPU monitoring using NtQuerySystemInformation() (Windows 2000/XP RTM fallback).";
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    // Get number of logical CPU cores
    const DWORD nCPUs = sysInfo.dwNumberOfProcessors;

    std::vector<SysProcPerfInfo> info(nCPUs);
    ULONG retLen = 0;
    // Create entries for all CPUs
    const ULONG num_cpus = static_cast<ULONG>(nCPUs * sizeof(SysProcPerfInfo));
    // SystemProcessorPerformanceInformation (class 0x08) returns one entry
    // per logical CPU.
    SYSTEM_INFORMATION_CLASS query = SystemProcessorPerformanceInformation;
    const NTSTATUS status = pfnNtQuery(query, info.data(),
                                       num_cpus, &retLen);
    if (status != STATUS_SUCCESS) {
      return 0.0f;
    }

    ULONGLONG total_idle   = 0;
    ULONGLONG total_kernel = 0;
    ULONGLONG total_user   = 0;
    for (DWORD i = 0; i < nCPUs; ++i) {
      total_idle   += static_cast<ULONGLONG>(info[i].IdleTime.QuadPart);
      total_kernel += static_cast<ULONGLONG>(info[i].KernelTime.QuadPart);
      total_user   += static_cast<ULONGLONG>(info[i].UserTime.QuadPart);
    }

    const ULONGLONG idle_delta   = total_idle   - prev_idle;
    const ULONGLONG kernel_delta = total_kernel - prev_kernel;
    const ULONGLONG user_delta   = total_user   - prev_user;

    prev_idle   = total_idle;
    prev_kernel = total_kernel;
    prev_user   = total_user;

    if (first_call) {
      first_call = false;
      return 0.0f;
    }

    const ULONGLONG total = kernel_delta + user_delta;
    if (total == 0) {
      return 0.0f;
    }
    const ULONGLONG busy_time = total - idle_delta;
    return std::clamp(
        static_cast<float>(busy_time) / static_cast<float>(total) * 100.0f,
        0.0f, 100.0f);
  }
}

// Validate GetCPUPercentImpl
const float GetCPUPercent() {
  const float cpu_percent = GetCPUPercentImpl();
  if (cpu_percent < 0.0f || cpu_percent > 100.0f) {
    LOG(FATAL) << L"GetCPUPercentImpl reported an out of bounds CPU %!";
  }
  return cpu_percent;
}

DWORD GetLogicalProcessorCount() {
  SYSTEM_INFO sysInfo;
  std::wstring whichfunc;
#if _WIN32_WINNT >= 0x0502 && defined(_WIN64)
  whichfunc = L"GetNativeSystemInfo";
  GetNativeSystemInfo(&sysInfo); // Directly run GetNativeSystemInfo
  pfnGetNativeSystemInfo = nullptr;
#else
  // Note: Do not call FreeLibrary() on GetModuleHandle() results
  HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
  if (!hKernel32) {
    // Fallback if kernel32.dll handle couldn't be obtained
    whichfunc = L"GetSystemInfo";
    GetSystemInfo(&sysInfo);
  } else {
    // Dynamically get GetNativeSystemInfo
    pfnGetNativeSystemInfo =
        reinterpret_cast<GetNativeSystemInfo_t>(GetProcAddress(hKernel32, "GetNativeSystemInfo"));
    // Windows 2000 won't have this function, use GetSystemInfo instead
    if (pfnGetNativeSystemInfo) {
      whichfunc = L"pfnGetNativeSystemInfo";
      pfnGetNativeSystemInfo(&sysInfo);
    } else {
      whichfunc = L"GetSystemInfo";
      GetSystemInfo(&sysInfo);
    }
  }
  if (debug_mode) {
    LOG(DEBUG) << L"Using " << whichfunc << " for " << __FUNC__;
  }
#endif
  const DWORD num_cpus = sysInfo.dwNumberOfProcessors;
  return num_cpus;
}
