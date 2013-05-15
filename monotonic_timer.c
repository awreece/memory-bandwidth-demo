// Copyright 2013 Alex Reece.

#include <unistd.h>

#define NANOS_PER_SECF 1000000000.0

#if _POSIX_TIMERS > 0 && _POSIX_MONOTONIC_CLOCK > 0
  // If we have it, use clock_gettime and CLOCK_MONOTONIC.

  #include <time.h>

  double monotonic_time() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return ((double) time.tv_sec) + ((double) time.tv_nsec / (NANOS_PER_SECF));
  }

#elif __APPLE__
  // If we don't have CLOCK_MONOTONIC, we might be on a Mac. There we instead
  // use mach_absolute_time().

  #include <mach/mach_time.h>

  static mach_timebase_info_data_t info;
  static void __attribute__((constructor)) init_info() {
    mach_timebase_info(&info);
  }

  double monotonic_time() {
    uint64_t time = mach_absolute_time();
    double dtime = (double) time;
    dtime *= (double) info.numer;
    dtime /= (double) info.denom;
    return dtime / NANOS_PER_SECF;
  }

// #elif _WIN64
// TODO(awreece) QueryPerformanceCounter and QueryPerformanceFrequency

#else
  #warning Falling back to rdtsc! Current implementation isn't satisfactory.

  // Fall back to rdtsc.
  static inline uint64_t rdtsc() {
    uint32_t hi, lo;
    // TODO(awreece) Use cpuid or rdtscp to serialize?
    // http://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf
    asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return (((uint64_t)hi) << 32) | (uint64_t)lo;
  }

  static uint64_t rdtsc_per_sec = 0;
  static void __attribute__((constructor)) init_rdtsc_per_sec() {
    uint64_t before, after;

    before = rdtsc();
    usleep(USECS_PER_SEC);
    after = rdtsc();

    rdtsc_per_sec = after - before;
  }

  double monotonic_time() {
    return (double) rdtsc / (double) rdtsc_per_sec;
  }

#endif
