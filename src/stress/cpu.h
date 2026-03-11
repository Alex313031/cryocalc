#ifndef CRYOCALC_CPU_H_
#define CRYOCALC_CPU_H_

#include <atomic>

#include "../framework.h"

typedef LONG (WINAPI* NtQuerySystemInformation_t)(UINT SystemInformationClass,
                                                  PVOID SystemInformation,
                                                  ULONG SystemInformationLength,
                                                  PULONG ReturnLength);

typedef BOOL (WINAPI* GetSystemTimes_t)(LPFILETIME lpIdleTime,
                                        LPFILETIME lpKernelTime,
                                        LPFILETIME lpUserTime);

extern std::atomic<bool> start_monitoring;

// Runs GetCPUPercentImpl, and verifies that it is between 0 and 100
const float GetCPUPercent();

// Set how many milliseconds to delay between CPU monitoring updates
void SetDelay(long cpu_monitor_delay);

// Runs GetCPUPercent and updates the CPU progress bar accordingly
void MonitorCPU();

// Turns MonitorCPU thread ON/OFF
void SetCPUMonitorState(bool on);

#endif // CRYOCALC_CPU_H_
