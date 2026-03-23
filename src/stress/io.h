#ifndef CRYOCALC_STRESS_IO_H_
#define CRYOCALC_STRESS_IO_H_

#include "common.h"

struct DiskPerfState {
  LONGLONG prev_read_time  = 0;
  LONGLONG prev_write_time = 0;
  LONGLONG prev_query_time = 0;
};

const float GetDiskIOPercent();

// Re-sample counters and update the internal PerfSnapshot io_percent.
// Call this once per timer tick before requesting the snapshot.
void UpdateIOPerfData();

#endif // CRYOCALC_STRESS_IO_H_
