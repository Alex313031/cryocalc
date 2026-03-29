#ifndef CRYOCALC_STRESS_COMMON_H_
#define CRYOCALC_STRESS_COMMON_H_

// clang-format off
#include "../framework.h"

#include <logging.h> // Keep this below framework.h

#ifndef NT_SUCCESS
 #define NT_SUCCESS(s) (((LONG)(s)) >= 0)
#endif

enum kMonType {
  CPU_TYPE  = 0,
  RAM_TYPE  = 1,
  COMM_TYPE = 2,
  IO_TYPE   = 3,
  MAX_TYPE  = 4
};
// clang-format on

typedef float ufloat;

// Snapshot of the most recently measured system performance counters.
struct PerfSnapshot {
  float cpu_percent;    // Total CPU utilization % (user + kernel, excluding idle)
  float kernel_percent; // Kernel-only CPU utilization % (excluding idle)
  float ram_percent;    // RAM utilization %
  float comm_percent;   // Commit Charge utilization %
  float io_percent;     // Disk I/O utilization %
  float ram_used_mb;    // Physical RAM currently in use (MB)
  float comm_used_mb;   // Commit charge currently in use (MB)
};

extern PerfSnapshot g_snapshot;

// Set once on first memory sample; never change at runtime.
extern float g_total_ram_mb;    // Total physical RAM (MB)
extern float g_total_commit_mb; // Total commit limit: RAM + pagefile (MB)

#endif // CRYOCALC_STRESS_COMMON_H_
