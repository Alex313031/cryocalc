#ifndef CRYOCALC_STRESS_IO_H_
#define CRYOCALC_STRESS_IO_H_

#include "common.h"

extern bool g_first_io_sample;

const float GetDiskIOPercent();

// Re-sample counters and update the internal PerfSnapshot io_percent.
// Call this once per timer tick before requesting the snapshot.
void UpdateIOPerfData();

#endif // CRYOCALC_STRESS_IO_H_
