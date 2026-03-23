#include "mem.h"

#include <os_info_dll.h>

#include "reporting.h"

static bool g_first_mem_sample = true; // Tracks whether this is first RAM/Commit Charge sample, for delta seeding

// Gets the current total RAM usage % and is supposed
// to return it as a float between 0.0 and 100.0.
// The ONLY caller of this function should be GetMemPercent.
static float GetMemPercentImpl() {
  static const bool legacy_fallback = g_legacy_fallback;

  if (g_GlobalMemoryStatusEx && !legacy_fallback) {
    MEMORYSTATUSEX mem_status;
    mem_status.dwLength = sizeof(mem_status);
    if (!g_GlobalMemoryStatusEx(&mem_status)) {
      return 0.0f;
    }
    return static_cast<float>(mem_status.dwMemoryLoad);
  } else {
    // Fallback for Windows 2000: GlobalMemoryStatus is available since Win95.
    MEMORYSTATUS mem_status;
    mem_status.dwLength = sizeof(mem_status);
    GlobalMemoryStatus(&mem_status);
    return static_cast<float>(mem_status.dwMemoryLoad);
  }
}

// Validate GetMemPercentImpl
const float GetMemPercent() {
  const float mem_percent = GetMemPercentImpl();
  if (mem_percent < 0.0f || mem_percent > 100.0f) {
    LOG(FATAL) << L"GetMemPercentImpl reported an out of bounds RAM %!";
  }
  return mem_percent;
}

// Gets the current total commit charge (RAM + Pagefile commit) % and is supposed
// to return it as a float between 0.0 and 100.0.
// The ONLY caller of this function should be GetCommitChargePercent.
static float GetCommitChargePercentImpl() {
  static const bool legacy_fallback = g_legacy_fallback;

  if (g_GlobalMemoryStatusEx && !legacy_fallback) {
    MEMORYSTATUSEX mem_status;
    mem_status.dwLength = sizeof(mem_status);
    if (!g_GlobalMemoryStatusEx(&mem_status)) {
      return 0.0f;
    }
    const DWORDLONG used = mem_status.ullTotalPageFile - mem_status.ullAvailPageFile;
    return static_cast<float>(used) / static_cast<float>(mem_status.ullTotalPageFile) * 100.0f;
  } else {
    // Fallback for Windows 2000: GlobalMemoryStatus is available since Win95.
    MEMORYSTATUS mem_status;
    mem_status.dwLength = sizeof(mem_status);
    GlobalMemoryStatus(&mem_status);
    const DWORD used = mem_status.dwTotalPageFile - mem_status.dwAvailPageFile;
    return static_cast<float>(used) / static_cast<float>(mem_status.dwTotalPageFile) * 100.0f;
  }
}

// Validate GetMemPercentImpl
const float GetCommitChargePercent() {
  static const bool isWine = IsRunningOnWine();
  const float commit_percent = GetCommitChargePercentImpl();
  if (commit_percent < 0.0f || commit_percent > 100.0f) {
    static bool winelogged = false;
    if (isWine) {
      float dummypercent = 255.0f;
      if (!winelogged) {
        LOG(ERROR) << L"WINE GetCommitChargePercentImpl reported an out of bounds Commit Charge: " << commit_percent;
      }
      if (commit_percent > 100.0f) {
        //CLOG(ERROR) << L"Setting WINE Commit Charge to 100%";
        dummypercent = 100.0f;
      } else if (commit_percent < 0.0f) {
        CLOG(ERROR) << L"Setting WINE Commit Charge to 0%";
        dummypercent = 0.0f;
      }
      winelogged = true;
      return dummypercent;
    } else {
      LOG(FATAL) << L"GetCommitChargePercentImpl reported an out of bounds Commit Charge: " << commit_percent;
    }
  }
  return commit_percent;
}

void UpdateMemPerfData() {
  g_first_mem_sample = false;
}
