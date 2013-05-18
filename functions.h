// Copyright 2013 Alex Reece.
//
// Functions fro writing to / reading from memory.

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <stddef.h>

#ifdef __SSE4_1__
void write_memory_nontemporal_sse(void*, size_t);
void write_memory_sse(void*, size_t);
void read_memory_sse(void*, size_t);
#endif

#ifdef __AVX__
void write_memory_nontemporal_avx(void*, size_t);
void write_memory_avx(void*, size_t);
void read_memory_avx(void*, size_t);
void read_memory_prefetch_avx(void*, size_t);
#endif

void write_memory_loop(void*, size_t);
void write_memory_rep_stosl(void*, size_t);
void read_memory_loop(void*, size_t);
void read_memory_rep_lodsl(void*, size_t);
void write_memory_memset(void*, size_t);

#endif  // FUNCTIONS_H_
