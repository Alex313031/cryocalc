#ifndef CRYOCALC_STRESS_MEM_H_
#define CRYOCALC_STRESS_MEM_H_

#include "../framework.h"

typedef BOOL (WINAPI* GlobalMemoryStatusEx_t)(MEMORYSTATUSEX*);

// Get total system memory usage
const float GetMemPercent();

#endif // CRYOCALC_STRESS_MEM_H_
