Memory Bandwidth Demo
=====================

This project was written to support my quest to achieve the theoretical best memory bandwidth for reads 
and writes on my machine, as described in [my blog post](http://www.codearcana.com/posts/2013/05/18/achieving-maximum-memory-bandwidth.html).
For a [Retina Macbook Pro](http://ark.intel.com/products/64891/Intel-Core-i7-3720QM-Processor-6M-Cache-up-to-3_60-GHz), 
I expect 25.6 GB/s (23.8 GiB/s). 

I've tried a number of approaches:

-   `read_memory_loop` does a simple `for (i = 0; i < size; *i++);`
-   `read_memory_sse` uses SSE packed aligned loads to read 16 bytes at a time.
-   `read_memory_avx` use AVX packed aligned stores to read 32 bytes at a time.
-   `write_memory_loop` does a simple `for (i = 0; i < size; *i++ = 1);`
-   `write_memory_rep_stosq` forces the use of the `rep stosq` instruction.
-   `write_memory_sse` uses SSE packed aligned stores to write 16 bytes at a 
    time.
-   `write_memory_nontemporal_sse` uses nontemporal SSE packed aligned stores to
    write 16 bytes at a time and bypass the cache.
-   `write_memory_avx` uses AVX packed aligned stores to write 32 bytes at a 
    time.
-   `write_memory_nontemporal_avx` uses nontemporal AVX packed aligned stores to
    write 32 bytes at a time and bypass the cache.
-   `write_memory_memset` is merely a wrapper for `memset`.

In addition, I tried wrapping all the above in OpenMP to use multiple cores. 
The function `*_omp` represent the OpenMP wrapped function `*`. To enable
this, compile the flags `-DWITH_OPENMP -fopenmp`.

Compiling
---------

Compiling this code requires a reasonably advanced version of `gcc` or `clang` 
(although `clang` does not support OpenMP).

Results
-------

~~~
./memory_profiler
           read_memory_rep_lodsl:  4.80 GiB/s
                read_memory_loop: 10.66 GiB/s
                 read_memory_sse: 13.44 GiB/s
                 read_memory_avx: 13.60 GiB/s
        read_memory_prefetch_avx: 15.06 GiB/s
               write_memory_loop: 12.84 GiB/s
          write_memory_rep_stosl: 19.22 GiB/s
                write_memory_sse:  8.93 GiB/s
    write_memory_nontemporal_sse: 12.83 GiB/s
                write_memory_avx:  8.91 GiB/s
    write_memory_nontemporal_avx: 12.65 GiB/s
             write_memory_memset: 12.84 GiB/s
       read_memory_rep_lodsl_omp: 19.01 GiB/s
            read_memory_loop_omp: 22.03 GiB/s
             read_memory_sse_omp: 22.18 GiB/s
             read_memory_avx_omp: 22.21 GiB/s
    read_memory_prefetch_avx_omp: 22.19 GiB/s
           write_memory_loop_omp: 22.13 GiB/s
      write_memory_rep_stosl_omp: 21.25 GiB/s
            write_memory_sse_omp:  9.70 GiB/s
write_memory_nontemporal_sse_omp: 22.13 GiB/s
            write_memory_avx_omp:  9.70 GiB/s
write_memory_nontemporal_avx_omp: 22.13 GiB/s
         write_memory_memset_omp: 22.14 GiB/s
~~~
