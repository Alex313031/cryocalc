#ifndef CRYOCALC_STRESS_MEM_H_
#define CRYOCALC_STRESS_MEM_H_

#include "common.h"

typedef BOOL (WINAPI* GlobalMemoryStatusEx_t)(MEMORYSTATUSEX*);

// Get total system memory usage
const float GetMemPercent();

const float GetCommitChargePercent();

#endif // CRYOCALC_STRESS_MEM_H_
