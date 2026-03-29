#ifndef CRYOCALC_STRESS_MEM_H_
#define CRYOCALC_STRESS_MEM_H_

#include "common.h"

typedef BOOL(WINAPI* GlobalMemoryStatusEx_t)(MEMORYSTATUSEX*);

extern bool g_first_mem_sample;

extern GlobalMemoryStatusEx_t g_GlobalMemoryStatusEx;

// Get total system memory usage
const float GetMemPercent();

const float GetCommitChargePercent();

// Re-sample counters and update the internal PerfSnapshot ram_percent and commit_percent.
// Call this once per timer tick before requesting the snapshot.
void UpdateMemPerfData();

#endif // CRYOCALC_STRESS_MEM_H_
