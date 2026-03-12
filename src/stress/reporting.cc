#include "reporting.h"

std::atomic<bool> start_monitoring{false};

long kCPUMonDelay = 1000L; // Default to 1 second

// Set system monitoring interval delay
void SetDelay(long cpu_monitor_delay) {
  kCPUMonDelay = cpu_monitor_delay;
}

// Get sys monitoring interval delay
const std::chrono::milliseconds GetDelay() {
  return std::chrono::milliseconds(kCPUMonDelay);
}

void MonitorCPU() {
  const std::chrono::milliseconds delay = GetDelay();
  while (start_monitoring) {
    float current_cpu_percent = GetCPUPercent(); // Get validated CPU percent
    SetCPUBarPos(current_cpu_percent); // Update CPU progress bar to nearest 1 percent
    std::this_thread::sleep_for(delay); // Pause between updates.
  }
  // start_monitoring was false, returning.
  LOG(INFO) << L"Stopping CPU Monitoring";
  return;
}

void SetCPUMonitorState(bool on) {
  start_monitoring = on;
}
