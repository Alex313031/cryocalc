#include "stress.h"

#include <emmintrin.h>
#include <logging.h>
#include <os_info_dll.h>

#include <algorithm>
#include <random>

std::atomic<bool> running{false};

bool use_sse2_simd = false;

volatile long double sse2_result = 0;
volatile long double vec_result  = 0;

static bool is_running = false;

static volatile unsigned int rand_seed;

// Whether to force using std::random
static constexpr bool force_rand_s = false;

void set_run_state(bool on) {
  running = on;
}

bool GetIsRunning() {
  return running || is_running;
}

void set_use_sse2(bool on) {
  use_sse2_simd = on;
}

static unsigned int GetSeed() {
  unsigned int seed      = 0;
  static const bool isXP = IsAtLeast(kWinXP);
  static const bool is2K = IsAtMost(kWin2000);

  if (isXP || force_rand_s) {
    if (is_debug) {
      LOG(DEBUG) << L"Using std::random_device for " << __FUNC__;
    }
    // Causes rand_s() crash on Windows 2000 because it uses RtlGenRandom internally. Bug in MinGW.
    std::random_device randd;
    seed = static_cast<unsigned int>(randd());
  } else if (is2K) {
    if (is_debug) {
      LOG(DEBUG) << L"Using CryptGenRandom for " << __FUNC__;
    }
    HCRYPTPROV hProvider    = 0;
    unsigned int win2k_seed = 0;

    // CryptGenRandom is available on Windows 2000+ (unlike RtlGenRandom/rand_s which requires XP+)
    if (CryptAcquireContextW(&hProvider, nullptr, nullptr, PROV_RSA_FULL,
                             CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
      CryptGenRandom(hProvider, sizeof(seed), (BYTE*)(&win2k_seed));
      CryptReleaseContext(hProvider, 0);
    }
    seed = win2k_seed;
  }
  CHECK(seed != 0);
  return seed;
}

// CPU-intensive stress test function  NOTE: Partially written by Claude AI!
void StressCPUVec(const size_t cache_size) {
  const size_t VECTOR_SIZE = 1024u * cache_size; // 1M elements to thrash L2/L3 cache
  unsigned int mem_multiplier;
  if (cache_size <= 1024u) {
    mem_multiplier = 4u;
  } else if (cache_size > 1024u && cache_size <= 3072u) {
    mem_multiplier = 2u;
  } else if (cache_size > 3072u && cache_size <= 4096u) {
    mem_multiplier = 1u;
  } else {
    mem_multiplier = 1u;
  }

  const size_t NUM_VECTORS =
      static_cast<size_t>(mem_multiplier); // Multiple vectors to increase memory pressure

  std::vector<std::vector<double>> data(NUM_VECTORS);
  for (auto& vec : data) {
    vec.resize(VECTOR_SIZE);
  }

  // Initialize with random data
  std::mt19937 gen(rand_seed);
  std::uniform_real_distribution<double> dis(0.0, 1024.0);

  for (auto& vec : data) {
    for (size_t i = 0; i < VECTOR_SIZE; ++i) {
      vec[i] = dis(gen);
    }
  }

  size_t iteration   = 0;
  long double result = 0;
  while (running) {
    // Perform computationally expensive operations
    for (size_t v = 0; v < NUM_VECTORS; ++v) {
      auto& vec = data[v];

      // Complex mathematical operations to max out ALU/FPU
      for (size_t i = 0; i < VECTOR_SIZE; ++i) {
        double val = vec[i];

        // Chain of dependent operations to prevent optimization
        val = std::sin(val) * std::cos(val);
        val = std::sqrt(std::abs(val)) + std::log(std::abs(val) + 1.0);
        val = std::pow(val, 1.5) * std::exp(val * 0.001);
        val = std::tan(val * 0.1) + std::atan(val);

        // More integer operations
        size_t idx = static_cast<size_t>(std::abs(val)) % VECTOR_SIZE;
        vec[i]     = val + vec[idx] * 0.5;
        if (!std::isfinite(vec[i])) {
          vec[i] = dis(gen);
        }
      }
    }

    // Cross-vector operations to thrash cache
    for (size_t i = 0; i < VECTOR_SIZE; i += 64u) {
      double sum = 0.0;
      for (size_t v = 0; v < NUM_VECTORS; ++v) {
        sum += data[v][i];
      }
      data[0][i] = std::isfinite(sum) ? sum : dis(gen);
    }

    // Matrix-like operations
    for (size_t v1 = 0; v1 < NUM_VECTORS - 1; ++v1) {
      for (size_t v2 = v1 + 1; v2 < NUM_VECTORS; ++v2) {
        for (size_t i = 0; i < VECTOR_SIZE; i += 128u) {
          double mval = data[v1][i] * data[v2][i] + std::sin(data[v1][i]) * std::cos(data[v2][i]);
          data[v1][i] = std::isfinite(mval) ? mval : dis(gen);
        }
      }
    }

    // Sorting to add branch misprediction pressure
    if (iteration % 10 == 0) {
      std::sort(data[iteration % NUM_VECTORS].begin(), data[iteration % NUM_VECTORS].end());
    }
    result = static_cast<long double>(data[0].front());

    iteration++;
  }
  vec_result = result;
}

// Version with inline SSE2 assembly for x86-64 (optional) NOTE: Written by Claude AI!
void StressCPUSSE2(const size_t cache_size) {
  const size_t VECTOR_SIZE = 1024u * cache_size;
  std::vector<double> data(VECTOR_SIZE);

  std::mt19937 gen(rand_seed);
  std::uniform_real_distribution<double> dis(0.0, 1024.0);

  for (size_t i = 0; i < VECTOR_SIZE; ++i) {
    data[i] = dis(gen);
  }
  long double result = 0;
  while (running) {
    for (size_t i = 0; i < VECTOR_SIZE - 4; i += 2) {
      // Use SSE2 intrinsics (128-bit operations, 2 doubles at a time)
      __m128d a = _mm_loadu_pd(&data[i]);
      __m128d b = _mm_loadu_pd(&data[i + 2]);
      // Multiple SSE2 operations (no FMA in SSE2, so use mul+add)
      // clang-format off
      for (int j = 0; j < 100; ++j) {
          __m128d temp_a = _mm_mul_pd(a, b);
          a = _mm_add_pd(temp_a, a);
          __m128d temp_b = _mm_mul_pd(b, a);
          b = _mm_add_pd(temp_b, b);
          // Additional operations to stress the CPU more
          a = _mm_div_pd(a, _mm_set1_pd(1.0001));
          b = _mm_sqrt_pd(_mm_mul_pd(b, _mm_set1_pd(0.9999)));
      }
      _mm_storeu_pd(&data[i], a);
      _mm_storeu_pd(&data[i + 2], b);
      for (int k = 0; k < 4; ++k)
        if (!std::isfinite(data[i + k]))
          data[i + k] = dis(gen);
      // clang-format on
    }
    result = static_cast<long double>(data.front());
  }
  // Return last fully calculated result.
  sse2_result = result;
}

long double GetSSE2Result() {
  return sse2_result;
}

long double GetVecResult() {
  return vec_result;
}

// Launches a vector of specified number of CreateThread
bool LaunchThreads(const unsigned int num_threads) {
  std::vector<std::thread> threads;
  if (num_threads == 0) {
    set_run_state(false);
    return false;
  } else {
    threads.reserve(num_threads);
    if (!running) {
      LOG(ERROR) << L"Must run set_run_state(true) before calling " << __FUNC__;
      return false;
    } else if (is_running) {
      LOG(ERROR) << L"Threads are already running!";
      return false;
    } else {
      is_running = true;
      rand_seed  = GetSeed();
      LOG(DEBUG) << L"Random seed = " << rand_seed;
      // std::vector<HANDLE> thread_handles;
      // thread_handles.push_back(CreateThread());
      //  Create threads
      const size_t cache_bytes = GetCacheSize();
      if (use_sse2_simd) {
        LOG(DEBUG) << L"Using SSE2 assembly stressor function";
        for (unsigned int i = 0; i < num_threads; ++i) {
          threads.emplace_back(StressCPUSSE2, cache_bytes);
        }
      } else {
        LOG(DEBUG) << L"Using vector math stressor function";
        for (unsigned int i = 0; i < num_threads; ++i) {
          threads.emplace_back(StressCPUVec, cache_bytes);
        }
      }
      LOG(INFO) << L"Using " << std::to_wstring(cache_bytes).c_str() << L"KB for cache size.";
      // Wait for threads to finish (this will never happen unless `running` is set to false)
      for (auto& thread : threads) {
        thread.join();
      }
      // All threads have exited and written their results: safe to read now
      if (use_sse2_simd) {
        CLOG(INFO) << L"SSE2 result = " << GetSSE2Result();
      } else {
        CLOG(INFO) << L"Vector result = " << GetVecResult();
      }
      return true;
    }
  }
}

void StopAllThreads() {
  bool post_msg = false;
  if (!is_running) {
    LOG(WARN) << L"No threads to stop: Queue empty.";
  } else {
    post_msg = true;
  }
  is_running = false;
  set_run_state(false);
  if (post_msg) {
    LOG(INFO) << L"Stopped all stressor threads.";
  }
}
