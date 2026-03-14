#include "io.h"

#include <os_info_dll.h>

#include "reporting.h"

// Gets the current total disk I/O usage % and is supposed
// to return it as a float between 0.0 and 100.0.
// The ONLY caller of this function should be GetDiskIOPercent.
static float GetDiskIOPercentImpl() {
  return 50.0f; // Dummy value for now. TODO add win32 getter
}

// Validate GetMemPercentImpl
const float GetDiskIOPercent() {
  const float io_percent = GetDiskIOPercentImpl();
  if (io_percent < 0.0f || io_percent > 100.0f) {
    LOG(FATAL) << L"GetDiskIOPercentImpl reported an out of bounds I/O %!";
  }
  return io_percent;
}
