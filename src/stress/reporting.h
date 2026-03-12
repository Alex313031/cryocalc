#ifndef CRYOCALC_STRESS_REPORTING_H_
#define CRYOCALC_STRESS_REPORTING_H_

#include "../framework.h"
#include "../ui_utils.h"

extern std::atomic<bool> start_monitoring;

// Set how many milliseconds to delay between CPU monitoring updates
void SetDelay(long cpu_monitor_delay);

// Get delay set by SetDelay()
const std::chrono::milliseconds GetDelay();

// Runs GetCPUPercent and updates the CPU progress bar accordingly
void MonitorCPU();

// Turns MonitorCPU thread ON/OFF
void SetCPUMonitorState(bool on);

#endif // CRYOCALC_STRESS_REPORTING_H_
