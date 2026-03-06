#ifndef CRYOCALC_STRESS_H_
#define CRYOCALC_STRESS_H_

#include "../framework.h"

// Volatile variables to be used by stress threads
extern volatile bool running; // For controlling thread activation state

extern volatile long double sse2_result;
extern volatile long double vec_result;

extern bool use_sse2_simd;    // For controlling which stress function to use.

// Call this before launching threads. Set to false to stop threads
void set_run_state(bool on);

// Get whether threads are running.
bool GetIsRunning();

// Set whether to use StressCPUSSE2
void set_use_sse2(bool on);

// Actual CPU stressor functions to be dispatched with threads.

// Matrix vector math to stress L2/L3 Caches.
//__attribute__((optimize("Og"))) void StressCPUVec(const size_t cache_size);
void StressCPUVec(const size_t cache_size);

// Inline SSE2 assembly to stress SIMD registers.
void StressCPUSSE2(const size_t cache_size);

// Uses CreateThread to start specified number of stressor threads.
bool LaunchThreads(const unsigned int num_threads);

// Stops all stressor threads
void StopAllThreads();

#endif // CRYOCALC_STRESS_H_
