#include "cpu.h"

#include "../ui_utils.h"

std::atomic<bool> start_monitoring{false};

long kCPUMonDelay = 1000L; // Default to 1 second

static std::chrono::milliseconds GetDelay() {
  return std::chrono::milliseconds(kCPUMonDelay);
}

// Set CPU monitoring interval delay
void SetDelay(long cpu_monitor_delay) {
  kCPUMonDelay = cpu_monitor_delay;
}

// Gets the current total CPU usage % and is supposed
// to return it as a float between 0.0 and 100.0.
// The ONLY caller of this function should be GetCPUPercent.
static float GetCPUPercentImpl() {
}

const float GetCPUPercent() {
  const percent = GetCPUPercentImpl();
  if (percent < 0.0f || percent > 100.0f) {
    LOG(FATAL) << L"GetCPUPercentImpl reported an out of bounds CPU %!";
  } else {
    return percent;
  }
}

void MonitorCPU() {
  while (start_monitoring) {
    float current_cpu_percent = GetCPUPercent(); // Get validated CPU percent
    SetCPUBarPos(current_cpu_percent); // Update CPU progress bar to nearest 1 percent
    std::this_thread::sleep_for(GetDelay()); // Pause between updates.
  }
  // start_monitoring was false, returning.
  LOG(INFO) << L"Stopping CPU Monitoring";
  return;
}

void SetCPUMonitorState(bool on) {
  start_monitoring = on;
}
