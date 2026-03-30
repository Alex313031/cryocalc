#ifndef CRYOCALC_STRESS_MEM_H_
#define CRYOCALC_STRESS_MEM_H_

#include "common.h"

typedef BOOL(WINAPI* GlobalMemoryStatusEx_t)(MEMORYSTATUSEX*);

extern GlobalMemoryStatusEx_t g_GlobalMemoryStatusEx;

extern bool g_first_mem_sample;

// Get total system memory usage
const float GetMemPercent();

// Get commit charge (virtual memory) usage
const float GetCommitChargePercent();

// Re-sample counters and update the internal PerfSnapshot ram_percent and commit_percent.
// Call this once per timer tick before requesting the snapshot.
void UpdateMemPerfData();

// Util to fill memory with zeros, used to stress Virtual Memory.
errno_t AllocateMemory(const size_t num_bytes);

#endif // CRYOCALC_STRESS_MEM_H_
