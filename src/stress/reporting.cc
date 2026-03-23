#include "reporting.h"

#include <os_info_dll.h>

#include "cpu.h"
#include "io.h"
#include "mem.h"

NtQuerySystemInformation_t g_NtQSI = nullptr;
GetSystemTimes_t g_GetSystemTimes  = nullptr;
GlobalMemoryStatusEx_t g_GlobalMemoryStatusEx = nullptr;

bool g_legacy_fallback = false;

int g_num_cpus = 0;

PerfSnapshot g_snapshot = {};

static bool g_first_sample = true; // Tracks whether this is first sample, for delta seeding

bool perf_data_initialized = false;

std::atomic<bool> start_cpu_mon{false};
std::atomic<bool> start_mem_mon{false};
std::atomic<bool> start_commit_mon{false};
std::atomic<bool> start_io_mon{false};

long kCPUMonDelay = 1000L; // Default to 1 second

// Set system monitoring interval delay
void SetDelay(long monitor_delay) {
  kCPUMonDelay = monitor_delay;
}

// Get sys monitoring interval delay
const std::chrono::milliseconds GetDelay() {
  return std::chrono::milliseconds(kCPUMonDelay);
}

void MonitorCPU() {
  const std::chrono::milliseconds delay = GetDelay();
  while (start_cpu_mon) {
    float now_cpu_percent = GetCPUPercent(); // Get validated CPU percent
    const std::wstring cpu_out = std::to_wstring(static_cast<int>(std::round(now_cpu_percent))) + L"%";
    SetWindowTextW(hCPUPercent, cpu_out.c_str());
    SetCPUBarPos(now_cpu_percent); // Update CPU progress bar to nearest 1 percent
    std::this_thread::sleep_for(delay); // Pause between updates.
  }
  LOG(INFO) << L"Stopping CPU Monitoring";
  return; // start_cpu_mon was false, returning.
}

void MonitorMem() {
  const std::chrono::milliseconds delay = GetDelay();
  while (start_mem_mon) {
    float now_mem_percent = GetMemPercent(); // Get validated RAM percent
    const std::wstring mem_out = std::to_wstring(static_cast<int>(std::round(now_mem_percent))) + L"%";
    SetWindowTextW(hMemPercent, mem_out.c_str());
    SetMEMBarPos(now_mem_percent); // Update MEM progress bar to nearest 1 percent
    std::this_thread::sleep_for(delay);
  }
  LOG(INFO) << L"Stopping MEM Monitoring";
  return; // start_mem_mon was false, returning.
}

void MonitorCommitCharge() {
  const std::chrono::milliseconds delay = GetDelay();
  while (start_commit_mon) {
    float now_commit_charge_percent = GetCommitChargePercent(); // Get validated Commit Charge percent
    const std::wstring commit_out = std::to_wstring(static_cast<int>(std::round(now_commit_charge_percent))) + L"%";
    SetWindowTextW(hCommitPercent, commit_out.c_str());
    SetCommitBarPos(now_commit_charge_percent); // Update commit charge progress bar to nearest 1 percent
    std::this_thread::sleep_for(delay);
  }
  LOG(INFO) << L"Stopping Commit Charge Monitoring";
  return; // start_commit_mon was false, returning.
}

void MonitorIO() {
  const std::chrono::milliseconds delay = GetDelay();
  while (start_io_mon) {
    float now_io_percent = GetDiskIOPercent(); // Get validated I/O usage percent
    const std::wstring io_out = std::to_wstring(static_cast<int>(std::round(now_io_percent))) + L"%";
    SetWindowTextW(hIOPercent, io_out.c_str());
    SetIOBarPos(now_io_percent); // Update I/O progress bar to nearest 1 percent
    std::this_thread::sleep_for(delay);
  }
  LOG(INFO) << L"Stopping I/O Monitoring";
  return; // start_io_mon was false, returning.
}

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
  last_percent = cpu_percent;
  const int cpubar_position = static_cast<int>(std::round(cpu_percent));
  SendMessageW(hCPUBar, PBM_SETPOS, static_cast<WPARAM>(cpubar_position), 0);
}

void SetMEMBarPos(float mem_percent) {
  if (mem_percent < 0.0f || mem_percent > 100.0f) {
    LOG(ERROR) << __FUNC__ << L" mem_percent out of bounds! " << mem_percent;
    return;
  }
  DCHECK(hMEMBar);
  static float last_percent = 0.0f;
  if (std::abs(mem_percent - last_percent) <= 0.9f) {
    return;
  }
  last_percent = mem_percent;
  const int membar_position = static_cast<int>(std::round(mem_percent));
  SendMessageW(hMEMBar, PBM_SETPOS, static_cast<WPARAM>(membar_position), 0);
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
  last_percent = commit_percent;
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
  last_percent = io_percent;
  const int iobar_position = static_cast<int>(std::round(io_percent));
  SendMessageW(hIOBar, PBM_SETPOS, static_cast<WPARAM>(iobar_position), 0);
}

void SetCPUMonitorState(bool on) {
  start_cpu_mon = on;
}

void SetMemMonitorState(bool on) {
  start_mem_mon = on;
}

void SetCommitMonitorState(bool on) {
  start_commit_mon = on;
}

void SetIOMonitorState(bool on) {
  start_io_mon = on;
}

void StartMonitoring(const long update_delay) {
  SetDelay(update_delay); // Set delay for monitoring intervals
  StartCPUMon();
  StartMemMon();
  StartCommitMon();
  StartIOMon();
}

// TODO add more
bool CloseMonitorWindow() {
  if (hMonitorWin != nullptr) {
    PostMessageW(hMonitorWin, WM_CLOSE, 0, 0);
  }
  return hMonitorWin == nullptr;
}

void StopMonitoring() {
  // Stop monitoring in reverse order 
  SetIOMonitorState(false);
  SendMessageW(hIOBar, PBM_SETPOS, 0, 0);
  SetCommitMonitorState(false);
  SendMessageW(hCommitBar, PBM_SETPOS, 0, 0);
  SetMemMonitorState(false);
  SendMessageW(hMEMBar, PBM_SETPOS, 0, 0);
  SetCPUMonitorState(false); // Tells MonitorCPU to return, acting like CPUMonitorThread.join()
  SendMessageW(hCPUBar, PBM_SETPOS, 0, 0); // Reset position to 0
  CloseMonitorWindow();
  CleanupPerfData();
}

void StartCPUMon() {
  SetCPUMonitorState(true); // Should be called before running any further functions in cpu.cc
  std::thread CPUMonitorThread(MonitorCPU); // Start CPU Monitor thread
  CPUMonitorThread.detach(); // Make sure to join with SetCPUMonitorState(false) before exiting
}

void StartMemMon() {
  SetMemMonitorState(true);
  std::thread MemMonitorThread(MonitorMem); // Start monitoring RAM usage
  MemMonitorThread.detach();
}

void StartCommitMon() {
  SetCommitMonitorState(true);
  std::thread CommitMonitorThread(MonitorCommitCharge); // Start monitoring commit charge usage
  CommitMonitorThread.detach();
}

void StartIOMon() {
  SetIOMonitorState(true);
  std::thread IOMonitorThread(MonitorIO); // Start monitoring disk I/O usage
  IOMonitorThread.detach();
}

void PauseMonitoring() {
  SetCPUMonitorState(false);
  SetMemMonitorState(false);
  SetCommitMonitorState(false);
  SetIOMonitorState(false);
  // TODO: Progress bar states
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

  g_NtQSI =
      reinterpret_cast<NtQuerySystemInformation_t>(GetProcAddress(ntdll, "NtQuerySystemInformation"));
  if (!g_NtQSI) {
    ErrorBox(nullptr, L"NtQuerySystemInformation Error",
            L"Failed to load NtQuerySystemInformation from ntdll.dll. \nCPU usage will not be available.");
    return false;
  }

  // GetSystemTimes is XP SP1+ only; gracefully absent on Windows 2000.
  HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
  if (kernel32) {
    g_GetSystemTimes = reinterpret_cast<GetSystemTimes_t>(
        GetProcAddress(kernel32, "GetSystemTimes"));
    g_GlobalMemoryStatusEx =
        reinterpret_cast<GlobalMemoryStatusEx_t>(
            GetProcAddress(kernel32, "GlobalMemoryStatusEx"));
  }

  if (debug_mode) {
    if (g_GetSystemTimes && !g_legacy_fallback) {
      LOG(DEBUG) << L"CPU monitoring using GetSystemTimes() (Windows XP SP1+).";
    } else {
      LOG(DEBUG) << L"CPU monitoring using NtQuerySystemInformation() (Windows 2000/XP RTM fallback).";
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
    UpdateIOPerfData(); // Updates both RAM and Commit Charge
  }
  g_first_sample = false;
}

void CleanupPerfData() {
  // ntdll.dll and kernel32.dll are never unloaded; just null the pointers.
  g_NtQSI          = nullptr;
  g_GetSystemTimes  = nullptr;
  g_GlobalMemoryStatusEx = nullptr;
}
