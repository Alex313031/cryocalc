#include "stress.h"

// Maybe use std::atomic instead?
volatile unsigned long long stress_prime_result = 0;
volatile bool running = false;

DWORD WINAPI HogCPU() {
  while (running) {
    unsigned long long num = 2048LL;
    bool is_prime;
    unsigned long long result;

    // A simple prime number generator (which will use a lot of CPU cycles)
    for (unsigned long long i = 2; i < num; ++i) {
      if (num % i == 0) {
        result = num;
        is_prime = false;
        break;
      } else {
        result = num;
        is_prime = true;
        break;
      }
    }

    if (is_prime) {
      // If prime, add 1 and continue prime search
      num = num + 1;
    }
    stress_prime_result = result;
  }
  return static_cast<DWORD>(stress_prime_result);
}

void set_run_state(bool on) {
  if (on) {
    running = true;
  } else {
    running = false;
  }
}
