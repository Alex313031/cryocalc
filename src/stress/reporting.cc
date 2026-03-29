#include "reporting.h"

#include <os_info_dll.h>

#include "cpu.h"
#include "io.h"
#include "mem.h"
#include "../ui_utils.h"
#include "../utils.h"

NtQuerySystemInformation_t g_NtQSI            = nullptr;
GetSystemTimes_t g_GetSystemTimes             = nullptr;
GlobalMemoryStatusEx_t g_GlobalMemoryStatusEx = nullptr;

bool g_legacy_fallback = false;

int g_num_cpus = 0;

PerfSnapshot g_snapshot = {};

float g_total_ram_mb    = 0.0f;
float g_total_commit_mb = 0.0f;

static bool g_first_sample = true; // Tracks whether this is first sample, for delta seeding

bool perf_data_initialized = false;

void SetCPUBarPos(float cpu_percent) {
  if (cpu_percent < 0.0f || cpu_percent > 100.0f) {
    LOG(ERROR) << __FUNC__ << L" cpu_percent out of bounds! " << cpu_percent;
    return;
  }
  DCHECK(hCPUBar);
  static float last_percent = 0.0f;
  // If less than ~1%, don't bother with bar update.
  if (std::abs(cpu_percent - last_percent) <= 0.9f) {
    return;
  }
  last_percent              = cpu_percent;
  const int cpubar_position = static_cast<int>(std::round(cpu_percent));
  SendMessageW(hCPUBar, PBM_SETPOS, static_cast<WPARAM>(cpubar_position), 0);
}

void SetMEMBarPos(float mem_percent) {
  if (mem_percent < 0.0f || mem_percent > 100.0f) {
    LOG(ERROR) << __FUNC__ << L" mem_percent out of bounds! " << mem_percent;
    return;
  }
  DCHECK(hMemBar);
  static float last_percent = 0.0f;
  if (std::abs(mem_percent - last_percent) <= 0.9f) {
    return;
  }
  last_percent              = mem_percent;
  const int membar_position = static_cast<int>(std::round(mem_percent));
  SendMessageW(hMemBar, PBM_SETPOS, static_cast<WPARAM>(membar_position), 0);
}

void SetCommitBarPos(float commit_percent) {
  if (commit_percent < 0.0f || commit_percent > 100.0f) {
    LOG(ERROR) << __FUNC__ << L" commit_percent out of bounds! " << commit_percent;
    return;
  }
  DCHECK(hCommitBar);
  static float last_percent = 0.0f;
  if (std::abs(commit_percent - last_percent) <= 0.9f) {
    return;
  }
  last_percent                 = commit_percent;
  const int commitbar_position = static_cast<int>(std::round(commit_percent));
  SendMessageW(hCommitBar, PBM_SETPOS, static_cast<WPARAM>(commitbar_position), 0);
}

void SetIOBarPos(float io_percent) {
  if (io_percent < 0.0f || io_percent > 100.0f) {
    LOG(ERROR) << __FUNC__ << L" io_percent out of bounds! " << io_percent;
    return;
  }
  DCHECK(hIOBar);
  static float last_percent = 0.0f;
  if (std::abs(io_percent - last_percent) <= 0.9f) {
    return;
  }
  last_percent             = io_percent;
  const int iobar_position = static_cast<int>(std::round(io_percent));
  SendMessageW(hIOBar, PBM_SETPOS, static_cast<WPARAM>(iobar_position), 0);
}

static bool CloseMonitorWindow() {
  if (hMonitorWin != nullptr) {
    PostMessageW(hMonitorWin, WM_CLOSE, 0, 0);
  }
  return hMonitorWin == nullptr;
}

void StopMonitoring() {
  KillTimer(hMainWindow, kUpdateTimerId);
  SendMessageW(hCPUBar, PBM_SETPOS, 0, 0);
  SendMessageW(hMemBar, PBM_SETPOS, 0, 0);
  SendMessageW(hCommitBar, PBM_SETPOS, 0, 0);
  SendMessageW(hIOBar, PBM_SETPOS, 0, 0);
  CloseMonitorWindow();
  CleanupPerfData();
}

const PerfSnapshot& GetPerfSnapshot() {
  return g_snapshot;
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
  g_legacy_fallback = IsWinOlderThan(kWinXP);

  g_NtQSI = reinterpret_cast<NtQuerySystemInformation_t>(
      GetProcAddress(ntdll, "NtQuerySystemInformation"));
  if (!g_NtQSI) {
    ErrorBox(nullptr, L"NtQuerySystemInformation Error",
             L"Failed to load NtQuerySystemInformation from ntdll.dll. \nCPU usage will not be "
             L"available.");
    return false;
  }

  // GetSystemTimes is XP SP1+ only; gracefully absent on Windows 2000.
  HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
  if (kernel32) {
    g_GetSystemTimes =
        reinterpret_cast<GetSystemTimes_t>(GetProcAddress(kernel32, "GetSystemTimes"));
    g_GlobalMemoryStatusEx =
        reinterpret_cast<GlobalMemoryStatusEx_t>(GetProcAddress(kernel32, "GlobalMemoryStatusEx"));
  }

  if (debug_mode) {
    if (g_GetSystemTimes && !g_legacy_fallback) {
      LOG(DEBUG) << L"CPU monitoring using GetSystemTimes() (Windows XP SP1+).";
    } else {
      LOG(DEBUG)
          << L"CPU monitoring using NtQuerySystemInformation() (Windows 2000/XP RTM fallback).";
    }
    if (g_GlobalMemoryStatusEx && !g_legacy_fallback) {
      LOG(DEBUG) << L"RAM monitoring using GlobalMemoryStatusEx() (Windows XP+).";
    } else {
      LOG(DEBUG) << L"RAM monitoring using GlobalMemoryStatus() (Windows 2000 fallback).";
    }
  }

  g_num_cpus = static_cast<int>(GetLogicalProcessorCount());
  if (g_num_cpus < 1) {
    g_num_cpus = 1;
  }

  // Seed the previous-sample counters so the first timer tick yields a real
  // reading rather than 0%.
  UpdatePerfData(); // sets g_first_sample = false, seeds prev counters

  perf_data_initialized = true;
  return true;
}

void UpdatePerfData() {
  // Update all the performance counters
  if (!g_first_sample) {
    UpdateCPUPerfData();
    UpdateMemPerfData(); // Updates both RAM and Commit Charge
    UpdateIOPerfData();  // Updates both RAM and Commit Charge
  }
  g_first_sample = false;
}

void CleanupPerfData() {
  // ntdll.dll and kernel32.dll are never unloaded; just null the pointers.
  g_NtQSI                = nullptr;
  g_GetSystemTimes       = nullptr;
  g_GlobalMemoryStatusEx = nullptr;
}
