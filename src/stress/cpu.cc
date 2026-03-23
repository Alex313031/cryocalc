#include "cpu.h"

#include <os_info_dll.h>

#include "reporting.h"

static GetNativeSystemInfo_t pfnGetNativeSystemInfo = nullptr;

static NtQuerySystemInformation_t g_NtQSI         = nullptr;
static GetSystemTimes_t           g_GetSystemTimes = nullptr;
static bool                       g_legacy_fallback = false;

static int g_num_cpus = 0;

static bool g_first_sample = true; // Tracks whether this is first sample, for delta seeding

static ULONGLONG g_prev_idle   = 0;
static ULONGLONG g_prev_kernel = 0;
static ULONGLONG g_prev_user   = 0;

bool perf_data_initialized = false;

// Returns the current CPU usage % as a float in [0.0, 100.0].
// Calls UpdatePerfData() to take a fresh sample, then reads g_snapshot.
const float GetCPUPercent() {
  UpdatePerfData();
  const float cpu_percent = static_cast<float>(g_snapshot.cpu_percent);
  if (cpu_percent < 0.0f || cpu_percent > 100.0f) {
    LOG(FATAL) << L"UpdatePerfData reported an out of bounds CPU %!";
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

const bool IsPerfDataInitialized() {
  return perf_data_initialized;
}

bool InitPerfData() {
  HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
  if (!ntdll) {
    LOG(ERROR) << L"Failed to get ntdll.dll!";
    return false;
  }

  g_NtQSI =
      reinterpret_cast<NtQuerySystemInformation_t>(GetProcAddress(ntdll, "NtQuerySystemInformation"));
  if (!g_NtQSI) {
    WarnBox(nullptr, L"NtQuerySystemInformation Error",
            L"Failed to load NtQuerySystemInformation from ntdll.dll. \nCPU usage will not be available.");
    return false;
  }

  // GetSystemTimes is XP SP1+ only; gracefully absent on Windows 2000.
  HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
  if (kernel32) {
    g_GetSystemTimes = reinterpret_cast<GetSystemTimes_t>(
        GetProcAddress(kernel32, "GetSystemTimes"));
  }
  g_legacy_fallback = IsWinOlderThan(kWinXP);

  if (debug_mode) {
    if (g_GetSystemTimes && !g_legacy_fallback) {
      LOG(DEBUG) << L"CPU monitoring using GetSystemTimes() (Windows XP SP1+).";
    } else {
      LOG(DEBUG) << L"CPU monitoring using NtQuerySystemInformation() (Windows 2000/XP RTM fallback).";
    }
  }

  g_num_cpus = static_cast<int>(GetLogicalProcessorCount());
  if (g_num_cpus < 1) {
    g_num_cpus = 1;
  }

  // Seed the previous-sample counters so the first timer tick yields a real
  // CPU reading rather than 0%.
  //g_first_sample = true;
  UpdatePerfData(); // sets g_first_sample = false, seeds prev counters

  perf_data_initialized = true;
  return true;
}

// Re-samples all CPU counters and updates g_snapshot.cpu_percent.
// Preferred path: GetSystemTimes (XP SP1+). Fallback: NtQuerySystemInformation (Win2k).
// Both MonitorCPU() (via GetCPUPercent) and MonitorWindowProc (WM_TIMER) call this;
// the shared delta state means consecutive calls from either thread each compute the
// delta since the last call by either — acceptable for a CPU usage display.
void UpdatePerfData() {
  auto FileTimeToULL = [](const FILETIME& ft) -> ULONGLONG {
    return (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
  };

  ULONGLONG idle = 0, kernel = 0, user = 0;
  bool got_sample = false;

  if (g_GetSystemTimes && !g_legacy_fallback) {
    // Preferred path: Windows XP SP1+
    FILETIME idle_ft, kernel_ft, user_ft;
    if (g_GetSystemTimes(&idle_ft, &kernel_ft, &user_ft)) {
      idle       = FileTimeToULL(idle_ft);
      kernel     = FileTimeToULL(kernel_ft); // includes idle
      user       = FileTimeToULL(user_ft);
      got_sample = true;
    }
  } else if (g_NtQSI) {
    // Fallback: Windows 2000 / XP RTM
    std::vector<SysProcPerfInfo> info(static_cast<size_t>(g_num_cpus));
    ULONG ret_len = 0;
    LONG status = g_NtQSI(
        SystemProcessorPerformanceInformation,
        info.data(),
        static_cast<ULONG>(sizeof(SysProcPerfInfo) * static_cast<size_t>(g_num_cpus)),
        &ret_len);
    if (NT_SUCCESS(status)) {
      for (int i = 0; i < g_num_cpus; i++) {
        idle   += static_cast<ULONGLONG>(info[static_cast<size_t>(i)].IdleTime.QuadPart);
        kernel += static_cast<ULONGLONG>(info[static_cast<size_t>(i)].KernelTime.QuadPart);
        user   += static_cast<ULONGLONG>(info[static_cast<size_t>(i)].UserTime.QuadPart);
      }
      got_sample = true;
    }
  }

  if (got_sample) {
    if (!g_first_sample) {
      const ULONGLONG d_idle   = idle   - g_prev_idle;
      const ULONGLONG d_kernel = kernel - g_prev_kernel;
      const ULONGLONG d_user   = user   - g_prev_user;
      const ULONGLONG d_total  = d_kernel + d_user;
      if (d_total > 0) {
        // kernel includes idle, so busy = (kernel - idle) + user = total - idle
        const ULONGLONG busy = (d_idle <= d_total) ? (d_total - d_idle) : 0ULL;
        const int pct = static_cast<int>((busy * 100ULL) / d_total);
        g_snapshot.cpu_percent = std::clamp(pct, 0, 100);
      }
    }

    g_prev_idle    = idle;
    g_prev_kernel  = kernel;
    g_prev_user    = user;
    g_first_sample = false;
  }
}

void CleanupPerfData() {
  // ntdll.dll and kernel32.dll are never unloaded; just null the pointers.
  g_NtQSI          = nullptr;
  g_GetSystemTimes  = nullptr;
  g_legacy_fallback = false;
}
