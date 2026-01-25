#ifndef CRYOCALC_STRESS_H_
#define CRYOCALC_STRESS_H_

#include "../framework.h"
#include "../globals.h"

// Volatile variables to be used by stress threads
extern volatile unsigned long long stress_prime_result; // Ultimate returned result from thread joining
extern volatile bool running; // For controlling thread activation state

// Actual CPU stressor function to be dispatched with threads.
DWORD WINAPI HogCPU();

// Call this before launching threads. Set to false to stop threads
void set_run_state(bool on);

#endif // CRYOCALC_STRESS_H_
