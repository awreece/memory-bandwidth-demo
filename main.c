// Copyright 2013 Alex Reece.
//
// A simple memory bandwidth profiler.
//
// Each of the write_memory_* functions read from a 1GB array. Each of the
// write_memory_* writes to the 1GB array. The goal is to get the max memory
// bandwidth as advertised by the intel specs: 25.6 GB/s (http://goo.gl/r8Aab)

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "./functions.h"
#include "./monotonic_timer.h"

#define SAMPLES 5
#define BYTES_PER_GB (1024*1024*1024LL)
#define SIZE (1*BYTES_PER_GB)

// This must be at least 32 byte aligned to make some AVX instructions happy.
char array[SIZE] __attribute__((aligned (32)));

// Compute the bandwidth in GiB/s.
static inline double to_bw(size_t bytes, double secs) {
  double size_bytes = (double) bytes;
  double size_gb = size_bytes / ((double) BYTES_PER_GB);
  return size_gb / secs;
}

// Time a function, printing out time to perform the memory operation and
// the computed memory bandwidth.
#define timefun(f) timeit(f, #f)
void timeit(void (*function)(void*, size_t), char* name) {
  double min = INFINITY;
  size_t i;
  for (i = 0; i < SAMPLES; i++) {
    double before, after, total;

    before = monotonic_time();
    function(array, SIZE);
    after = monotonic_time();

    total = after - before;
    if (total < min) {
      min = total;
    }
  }

  printf("%30s: %5.2f GiB/s\n", name, to_bw(SIZE, min));
}

int main() {
  memset(array, 0xFF, SIZE);  // un-ZFOD the page.

  // TODO(awreece) iopl(0) and cli/sti?

  timefun(read_memory_loop);
#ifdef __SSE4_1__
  timefun(read_memory_sse);
#endif
#ifdef __AVX__
  timefun(read_memory_avx);
#endif

  timefun(write_memory_loop);
  timefun(write_memory_dillon);
  timefun(write_memory_rep_stosl);
#ifdef __SSE4_1__
  timefun(write_memory_sse);
  timefun(write_memory_nontemporal_sse);
#endif
#ifdef __AVX__
  timefun(write_memory_avx);
  timefun(write_memory_nontemporal_avx);
#endif
  timefun(write_memory_memset);

  return 0;
}
