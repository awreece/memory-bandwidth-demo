// Copyright 2013 Alex Reece.
//
// A simple memory bandwidth profiler.
//
// Each of the write_memory_* functions read from a 1GB array. Each of the
// write_memory_* writes to the 1GB array. The goal is to get the max memory
// bandwidth as advertised by the intel specs: 23.8 GiB/s (http://goo.gl/r8Aab)

#include <assert.h>
#include <math.h>
#ifdef WITH_OPENMP
#include <omp.h>
#endif  // WITH_OPENMP
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "./functions.h"
#include "./monotonic_timer.h"

#define SAMPLES 5
#define TIMES 5
#define BYTES_PER_GB (1024*1024*1024LL)
#define SIZE (1*BYTES_PER_GB)
#define PAGE_SIZE (1<<12)

// This must be at least 32 byte aligned to make some AVX instructions happy.
// Have PAGE_SIZE buffering so we don't have to do math for prefetching.
char array[SIZE + PAGE_SIZE] __attribute__((aligned (32)));

// Compute the bandwidth in GiB/s.
static inline double to_bw(size_t bytes, double secs) {
  double size_bytes = (double) bytes;
  double size_gb = size_bytes / ((double) BYTES_PER_GB);
  return size_gb / secs;
}

#ifdef WITH_OPENMP
// Time a function, printing out time to perform the memory operation and
// the computed memory bandwidth. Use openmp to do threading (set environment
// variable OMP_NUM_THREADS to control threads use.
#define timefunp(f) timeitp(f, #f)
void timeitp(void (*function)(void*, size_t), char* name) {
  double min = INFINITY;
  size_t i;
  for (i = 0; i < SAMPLES; i++) {
    double before, after, total;

    assert(SIZE % omp_get_max_threads() == 0);

    size_t chunk_size = SIZE / omp_get_max_threads();
#pragma omp parallel
    {
#pragma omp barrier
#pragma omp master
      before = monotonic_time();
      int j;
      for (j = 0; j < TIMES; j++) {
	function(&array[chunk_size * omp_get_thread_num()], chunk_size);
      }
#pragma omp barrier
#pragma omp master
      after = monotonic_time();
    }

    total = after - before;
    if (total < min) {
      min = total;
    }
  }

  printf("%28s_omp: %5.2f GiB/s\n", name, to_bw(SIZE * TIMES, min));
}
#endif  // WITH_OPENMP

// Time a function, printing out time to perform the memory operation and
// the computed memory bandwidth.
#define timefun(f) timeit(f, #f)
void timeit(void (*function)(void*, size_t), char* name) {
  double min = INFINITY;
  size_t i;
  for (i = 0; i < SAMPLES; i++) {
    double before, after, total;

    before = monotonic_time();
    int j;
    for (j = 0; j < TIMES; j++) {
      function(array, SIZE);
    }
    after = monotonic_time();

    total = after - before;
    if (total < min) {
      min = total;
    }
  }

  printf("%32s: %5.2f GiB/s\n", name, to_bw(SIZE * TIMES, min));
}

int main() {
  memset(array, 0xFF, SIZE);  // un-ZFOD the page.
  * ((uint64_t *) &array[SIZE]) = 0;

  // TODO(awreece) iopl(0) and cli/sti?

  timefun(read_memory_repne_scasl);
  timefun(read_memory_rep_lodsl);
  timefun(read_memory_loop);
#ifdef __SSE4_1__
  timefun(read_memory_sse);
#endif
#ifdef __AVX__
  timefun(read_memory_avx);
  timefun(read_memory_prefetch_avx);
#endif

  timefun(write_memory_loop);
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

#ifdef WITH_OPENMP

  memset(array, 0xFF, SIZE);  // un-ZFOD the page.
  * ((uint64_t *) &array[SIZE]) = 0;

  timefunp(read_memory_repne_scasl);
  timefunp(read_memory_rep_lodsl);
  timefunp(read_memory_loop);
#ifdef __SSE4_1__
  timefunp(read_memory_sse);
#endif
#ifdef __AVX__
  timefunp(read_memory_avx);
  timefunp(read_memory_prefetch_avx);
#endif

  timefunp(write_memory_loop);
  timefunp(write_memory_rep_stosl);
#ifdef __SSE4_1__
  timefunp(write_memory_sse);
  timefunp(write_memory_nontemporal_sse);
#endif
#ifdef __AVX__
  timefunp(write_memory_avx);
  timefunp(write_memory_nontemporal_avx);
#endif
  timefunp(write_memory_memset);

#endif  // WITH_OPENMP

  return 0;
}
