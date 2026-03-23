#include "io.h"

#include <winioctl.h>

#include "reporting.h"


// Gets the current total disk I/O usage % and is supposed
// to return it as a float between 0.0 and 100.0.
// The ONLY caller of this function should be GetDiskIOPercent.
static float GetDiskIOPercentImpl() {
  static bool first_call = true;
  static std::vector<DiskPerfState> disk_states;

  float max_percent = 0.0f;

  for (int i = 0; i < 16; ++i) {
    const std::wstring drive_path = L"\\\\.\\PhysicalDrive" + std::to_wstring(i);
    HANDLE hDisk = CreateFileW(drive_path.c_str(), 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_EXISTING, 0, nullptr);
    if (hDisk == INVALID_HANDLE_VALUE) {
      break;
    }

    DISK_PERFORMANCE perf = {};
    DWORD bytes_returned = 0;
    const BOOL ok = DeviceIoControl(hDisk, IOCTL_DISK_PERFORMANCE,
        nullptr, 0, &perf, sizeof(perf), &bytes_returned, nullptr);
    CloseHandle(hDisk);

    if (!ok) continue;

    if (i >= static_cast<int>(disk_states.size())) {
      disk_states.resize(i + 1);
    }

    DiskPerfState& state = disk_states[i];

    if (first_call) {
      state.prev_read_time  = perf.ReadTime.QuadPart;
      state.prev_write_time = perf.WriteTime.QuadPart;
      state.prev_query_time = perf.QueryTime.QuadPart;
      continue;
    }

    const LONGLONG read_delta  = perf.ReadTime.QuadPart  - state.prev_read_time;
    const LONGLONG write_delta = perf.WriteTime.QuadPart - state.prev_write_time;
    const LONGLONG query_delta = perf.QueryTime.QuadPart - state.prev_query_time;

    state.prev_read_time  = perf.ReadTime.QuadPart;
    state.prev_write_time = perf.WriteTime.QuadPart;
    state.prev_query_time = perf.QueryTime.QuadPart;

    if (query_delta <= 0) continue;

    const float percent = static_cast<float>(read_delta + write_delta) /
                          static_cast<float>(query_delta) * 100.0f;
    max_percent = std::max(max_percent, percent);
  }

  if (first_call) {
    first_call = false;
    return 0.0f;
  }

  return std::clamp(max_percent, 0.0f, 100.0f);
}

// Validate GetMemPercentImpl
const float GetDiskIOPercent() {
  const float io_percent = GetDiskIOPercentImpl();
  if (io_percent < 0.0f || io_percent > 100.0f) {
    LOG(FATAL) << L"GetDiskIOPercentImpl reported an out of bounds I/O %!";
  }
  return io_percent;
}

void UpdateIOPerfData() {
  g_snapshot.io_percent = GetDiskIOPercent();
}
