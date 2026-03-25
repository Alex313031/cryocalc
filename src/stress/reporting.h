#ifndef CRYOCALC_STRESS_REPORTING_H_
#define CRYOCALC_STRESS_REPORTING_H_

#include "common.h"
#include "../utils.h"
#include "../ui_utils.h"

// Whether to use older functions
extern bool g_legacy_fallback;

// Global number of logical CPUs
extern int g_num_cpus;

// Sets CPU progress bar position according to usage %
void SetCPUBarPos(float cpu_percent);

// Sets MEM progress bar position according to usage %
void SetMEMBarPos(float mem_percent);

// Sets commit charge progress bar position according to usage %
void SetCommitBarPos(float commit_percent);

// Sets I/O progress bar position according to usage %
void SetIOBarPos(float io_percent);

// Helper function to stop all monitoring
void StopMonitoring();

// Returns a const reference to the last snapshot produced by UpdatePerfData().
const PerfSnapshot& GetPerfSnapshot();

// Returns true if InitPerfData has been run at least once
const bool IsPerfDataInitialized();

// Dynamically load DLL function pointers and take initial
// counter samples so the first timer tick gives a real reading.
bool InitPerfData();

// Helper function to update global snapshot
void UpdatePerfData();

// Release any resources from InitPerfData().
void CleanupPerfData();

#endif // CRYOCALC_STRESS_REPORTING_H_
