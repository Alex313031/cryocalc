#include "mem.h"

#include <os_info_dll.h>

#include "reporting.h"
#include "../utils.h"

bool g_first_mem_sample = true;

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
    if (g_total_ram_mb == 0.0f) {
      g_total_ram_mb = static_cast<float>(mem_status.ullTotalPhys) / (1024.0f * 1024.0f);
    }
    g_snapshot.ram_used_mb =
        static_cast<float>(mem_status.ullTotalPhys - mem_status.ullAvailPhys) / (1024.0f * 1024.0f);
    return static_cast<float>(mem_status.dwMemoryLoad);
  } else {
    // Fallback for Windows 2000: GlobalMemoryStatus is available since Win95.
    MEMORYSTATUS mem_status;
    mem_status.dwLength = sizeof(mem_status);
    GlobalMemoryStatus(&mem_status);
    if (g_total_ram_mb == 0.0f) {
      g_total_ram_mb = static_cast<float>(mem_status.dwTotalPhys) / (1024.0f * 1024.0f);
    }
    g_snapshot.ram_used_mb =
        static_cast<float>(mem_status.dwTotalPhys - mem_status.dwAvailPhys) / (1024.0f * 1024.0f);
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
    if (g_total_commit_mb == 0.0f) {
      g_total_commit_mb = static_cast<float>(mem_status.ullTotalPageFile) / (1024.0f * 1024.0f);
    }
    const DWORDLONG used    = mem_status.ullTotalPageFile - mem_status.ullAvailPageFile;
    g_snapshot.comm_used_mb = static_cast<float>(used) / (1024.0f * 1024.0f);
    return static_cast<float>(used) / static_cast<float>(mem_status.ullTotalPageFile) * 100.0f;
  } else {
    // Fallback for Windows 2000: GlobalMemoryStatus is available since Win95.
    MEMORYSTATUS mem_status;
    mem_status.dwLength = sizeof(mem_status);
    GlobalMemoryStatus(&mem_status);
    if (g_total_commit_mb == 0.0f) {
      g_total_commit_mb = static_cast<float>(mem_status.dwTotalPageFile) / (1024.0f * 1024.0f);
    }
    const DWORD used        = mem_status.dwTotalPageFile - mem_status.dwAvailPageFile;
    g_snapshot.comm_used_mb = static_cast<float>(used) / (1024.0f * 1024.0f);
    return static_cast<float>(used) / static_cast<float>(mem_status.dwTotalPageFile) * 100.0f;
  }
}

// Validate GetMemPercentImpl
const float GetCommitChargePercent() {
  static const bool isWine   = IsRunningOnWine();
  const float commit_percent = GetCommitChargePercentImpl();
  if (commit_percent < 0.0f || commit_percent > 100.0f) {
    static bool winelogged = false;
    if (isWine) {
      float dummypercent = 255.0f;
      if (!winelogged) {
        LOG(ERROR) << L"WINE GetCommitChargePercentImpl reported an out of bounds Commit Charge: "
                   << commit_percent;
      }
      if (commit_percent > 100.0f) {
        // CLOG(ERROR) << L"Setting WINE Commit Charge to 100%";
        dummypercent = 100.0f;
      } else if (commit_percent < 0.0f) {
        CLOG(ERROR) << L"Setting WINE Commit Charge to 0%";
        dummypercent = 0.0f;
      }
      winelogged = true;
      return dummypercent;
    } else {
      LOG(FATAL) << L"GetCommitChargePercentImpl reported an out of bounds Commit Charge: "
                 << commit_percent;
    }
  }
  return commit_percent;
}

void UpdateMemPerfData() {
  const float mem    = GetMemPercent();
  const float commit = GetCommitChargePercent();
  if (mem > 0.0f && commit > 0.0f) {
    g_first_mem_sample = false;
  }
  if (!g_first_mem_sample) {
    g_snapshot.ram_percent  = mem;
    g_snapshot.comm_percent = commit;
  }
}

static void zeromem(void* dest, const size_t count) {
  ::memset(dest, 0, count);
}

errno_t AllocateMemory(const size_t num_bytes) {
  LPVOID pMemory = VirtualAlloc(nullptr, num_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (pMemory == NULL) {
    MessageBoxW(nullptr, L"Failed to allocate memory", L"VirtualAlloc Error", MB_OK | MB_ICONERROR);
    return 12; // ENOMEM
  } else {
    LOG(DEBUG) << static_cast<int>(num_bytes / 1048576u)
               << " Megabytes memory allocated at address: " << std::showbase << std::hex
               << reinterpret_cast<unsigned long long>(pMemory) << std::dec << std::noshowbase;
    return 0;
  }
}
