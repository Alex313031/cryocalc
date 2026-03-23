#ifndef CRYOCALC_STRESS_REPORTING_H_
#define CRYOCALC_STRESS_REPORTING_H_

#include <chrono>

#include "common.h"
#include "../utils.h"
#include "../ui_utils.h"

// Set how many milliseconds to delay between CPU monitoring updates
void SetDelay(long monitor_delay);

// Get delay set by SetDelay()
const std::chrono::milliseconds GetDelay();

// Runs GetCPUPercent and updates the CPU progress bar accordingly
void MonitorCPU();

// Runs GetMemPercent and updates the RAM progress bar accordingly
void MonitorMem();

// Runs GetCommitChargePercent and updates the Commit Charge progress bar accordingly
void MonitorCommitCharge();

// Runs GetDiskIOPercent and updates the I/O progress bar accordingly
void MonitorIO();

// Sets CPU progress bar position according to usage %
void SetCPUBarPos(float cpu_percent);

// Sets MEM progress bar position according to usage %
void SetMEMBarPos(float mem_percent);

// Sets commit charge progress bar position according to usage %
void SetCommitBarPos(float commit_percent);

// Sets I/O progress bar position according to usage %
void SetIOBarPos(float io_percent);

// Turns MonitorCPU thread ON/OFF
void SetCPUMonitorState(bool on);

// Turns MonitorMem thread ON/OFF
void SetMemMonitorState(bool on);

// Turns MonitorCommitCharge thread ON/OFF
void SetCommitMonitorState(bool on);

// Turns MonitorIO thread ON/OFF
void SetIOMonitorState(bool on);

// Helper function to start MonitorCPU and MonitorMem with the specified delay
void StartMonitoring(const long update_delay);

// Helper function to stop all monitoring
void StopMonitoring();

// Helper function to start CPU Monitoring thread
void StartCPUMon();

// Helper function to start RAM Monitoring thread
void StartMemMon();

// Helper function to start Commit Charge Monitoring thread
void StartCommitMon();

// Helper function to start I/O Monitoring thread
void StartIOMon();

// Helper function to stop monitoring, but keep hCPUBar/hMEMBar current position
void PauseMonitoring();

#endif // CRYOCALC_STRESS_REPORTING_H_
