#include "reporting.h"

#include "cpu.h"
#include "mem.h"

std::atomic<bool> start_cpu_mon{false};
std::atomic<bool> start_mem_mon{false};

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
    float current_cpu_percent = GetCPUPercent(); // Get validated CPU percent
    SetCPUBarPos(current_cpu_percent); // Update CPU progress bar to nearest 1 percent
    std::this_thread::sleep_for(delay); // Pause between updates.
  }
  // start_cpu_mon was false, returning.
  LOG(INFO) << L"Stopping CPU Monitoring";
  return;
}

void MonitorMem() {
  const std::chrono::milliseconds delay = GetDelay();
  while (start_mem_mon) {
    float current_mem_percent = GetMemPercent(); // Get validated RAM percent
    SetMEMBarPos(current_mem_percent); // Update MEM progress bar to nearest 1 percent
    std::this_thread::sleep_for(delay);
  }
  // start_mem_mon was false, returning.
  LOG(INFO) << L"Stopping MEM Monitoring";
  return;
}

void SetCPUBarPos(float cpu_percent) {
  if (cpu_percent < 0.0f || cpu_percent > 100.0f) {
    LOG(ERROR) << __FUNC__ << L" cpu_percent out of bounds! " << cpu_percent;
    return;
  }
  DCHECK(hCPUBar);
  const int position = static_cast<int>(std::round(cpu_percent));
  SendMessageW(hCPUBar, PBM_SETPOS, static_cast<WPARAM>(position), 0);
}

void SetMEMBarPos(float mem_percent) {
  if (mem_percent < 0.0f || mem_percent > 100.0f) {
    LOG(ERROR) << __FUNC__ << L" mem_percent out of bounds! " << mem_percent;
    return;
  }
  DCHECK(hMEMBar);
  const int position = static_cast<int>(std::round(mem_percent));
  SendMessageW(hMEMBar, PBM_SETPOS, static_cast<WPARAM>(position), 0);
}

void SetCPUMonitorState(bool on) {
  start_cpu_mon = on;
}

void SetMemMonitorState(bool on) {
  start_mem_mon = on;
}

void StartMonitoring(const long update_delay) {
  SetDelay(update_delay); // Set delay for monitoring intervals
  StartCPUMon();
  StartMemMon();
}

void StopMonitoring() {
  StopMemMon();
  StopCPUMon();
}

void StartCPUMon() {
  SetCPUMonitorState(true); // Should be called before running any further functions in cpu.cc
  std::thread CPUMonitorThread(MonitorCPU); // Start CPU Monitor thread
  CPUMonitorThread.detach(); // Make sure to join with SetCPUMonitorState(false) before exiting
}

void StopCPUMon() {
  SetCPUMonitorState(false); // Tells MonitorCPU to return, acting like CPUMonitorThread.join()
  SendMessageW(hCPUBar, PBM_SETPOS, 0, 0); // Reset position to 0
}

void StartMemMon() {
  SetMemMonitorState(true);
  std::thread MemMonitorThread(MonitorMem); // Start monitoring RAM usage
  MemMonitorThread.detach();
}

void StopMemMon() {
  SetMemMonitorState(false);
  SendMessageW(hMEMBar, PBM_SETPOS, 0, 0);
}

void PauseMonitoring() {
  SetCPUMonitorState(false);
  SetMemMonitorState(false);
  // TODO: Progress bar states
}
