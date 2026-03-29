#include "cpu.h"

#include "reporting.h"
#include "../utils.h"

static GetNativeSystemInfo_t pfnGetNativeSystemInfo = nullptr;

static bool g_first_cpu_sample = true; // Tracks whether this is first CPU sample, for delta seeding

static ULONGLONG g_prev_idle   = 0;
static ULONGLONG g_prev_kernel = 0;
static ULONGLONG g_prev_user   = 0;

// Returns the current CPU usage % as a float in [0.0, 100.0].
// Calls UpdateCPUPerfData() to take a fresh sample, then reads g_snapshot.cpu_percent.
const float GetCPUPercent() {
  UpdateCPUPerfData();
  const float cpu_percent = g_snapshot.cpu_percent;
  if (cpu_percent < 0.0f || cpu_percent > 100.0f) {
    LOG(FATAL) << L"UpdateCPUPerfData reported an out of bounds CPU %!";
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

// Re-samples all CPU counters and updates g_snapshot.cpu_percent.
// Preferred path: GetSystemTimes (XP SP1+). Fallback: NtQuerySystemInformation (Win2k).
// Both MonitorCPU() (via GetCPUPercent) and MonitorWindowProc (WM_TIMER) call this;
// the shared delta state means consecutive calls from either thread each compute the
// delta since the last call by either — acceptable for a CPU usage display.
void UpdateCPUPerfData() {
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
    LONG status   = g_NtQSI(
        SystemProcessorPerformanceInformation, info.data(),
        static_cast<ULONG>(sizeof(SysProcPerfInfo) * static_cast<size_t>(g_num_cpus)), &ret_len);
    if (NT_SUCCESS(status)) {
      for (int i = 0; i < g_num_cpus; i++) {
        idle += static_cast<ULONGLONG>(info[static_cast<size_t>(i)].IdleTime.QuadPart);
        kernel += static_cast<ULONGLONG>(info[static_cast<size_t>(i)].KernelTime.QuadPart);
        user += static_cast<ULONGLONG>(info[static_cast<size_t>(i)].UserTime.QuadPart);
      }
      got_sample = true;
    }
  }

  if (got_sample) {
    if (!g_first_cpu_sample) {
      const ULONGLONG d_idle   = idle - g_prev_idle;
      const ULONGLONG d_kernel = kernel - g_prev_kernel;
      const ULONGLONG d_user   = user - g_prev_user;
      const ULONGLONG d_total  = d_kernel + d_user;
      if (d_total > 0) {
        // kernel includes idle, so busy = (kernel - idle) + user = total - idle
        const ULONGLONG busy    = (d_idle <= d_total) ? (d_total - d_idle) : 0ULL;
        const ULONGLONG kbusy   = (d_idle <= d_kernel) ? (d_kernel - d_idle) : 0ULL;
        const float final_total = static_cast<float>(d_total);
        g_snapshot.cpu_percent =
            std::clamp(static_cast<float>(busy * 100ULL) / final_total, 0.0f, 100.0f);
        g_snapshot.kernel_percent =
            std::clamp(static_cast<float>(kbusy * 100ULL) / final_total, 0.0f, 100.0f);
      }
    }

    g_prev_idle        = idle;
    g_prev_kernel      = kernel;
    g_prev_user        = user;
    g_first_cpu_sample = false;
  }
}
