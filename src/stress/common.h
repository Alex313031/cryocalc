#ifndef CRYOCALC_STRESS_COMMON_H_
#define CRYOCALC_STRESS_COMMON_H_

#include <atomic>

#include "../framework.h"

// Snapshot of the most recently measured system performance counters.
struct PerfSnapshot {
  int cpu_percent; // CPU utilization %
  float ram_percent; // RAM utilization %
  float comm_percent; // Commit Charge utilization %
  int io_percent; // Commit Charge utilization %
};

extern PerfSnapshot g_snapshot;

// For controlling sys monitoring threads activation
extern std::atomic<bool> start_cpu_mon;
extern std::atomic<bool> start_mem_mon;
extern std::atomic<bool> start_commit_mon;
extern std::atomic<bool> start_io_mon;

#endif // CRYOCALC_STRESS_COMMON_H_
