Memory Bandwidth Demo
=====================

I've been attempting to achieve the theoretical best memory bandwidth for reads 
and writes on my machine. For a [Retina Macbook Pro](http://ark.intel.com/products/64891/Intel-Core-i7-3720QM-Processor-6M-Cache-up-to-3_60-GHz), 
I expect 25.6 GB/s. 

I've tried a number of approaches:

-   `read_memory_loop` does a simple `for (i = 0; i < size; *i++);`
-   `read_memory_sse` uses SSE packed aligned loads to read 16 bytes at a time.
-   `read_memory_avx` use AVX packed aligned stores to read 32 bytes at a time.
-   `write_memory_loop` does a simple `for (i = 0; i < size; *i++ = 1);`
-   `write_memory_sse` uses SSE packed aligned stores to write 16 bytes at a 
    time.
-   `write_memory_nontemporal_sse` uses nontemporal SSE packed aligned stores to
    write 16 bytes at a time and bypass the cache.
-   `write_memory_avx` uses AVX packed aligned stores to write 32 bytes at a 
    time.
-   `write_memory_nontemporal_avx` uses nontemporal AVX packed aligned stores to
    write 32 bytes at a time and bypass the cache.
-   `write_memory_memset` is merely a wrapper for `memset`.

Profiling, I get:
~~~
$ ./memory_profiler
              read_memory_loop: 13.24 GiB/s
               read_memory_sse: 15.98 GiB/s
               read_memory_avx: 16.75 GiB/s
             write_memory_loop: 17.01 GiB/s
              write_memory_sse:  9.16 GiB/s
  write_memory_nontemporal_sse: 17.00 GiB/s
              write_memory_avx:  9.31 GiB/s
  write_memory_nontemporal_avx: 16.53 GiB/s
           write_memory_memset: 17.10 GiB/s
~~~

I don't get anywhere close to the full bandwidth, even on reads. Interestingly,
I tried to run in single user mode to eliminate any background bus traffic and
got results that were considerably lower than when I ran normally (I don't think
I fully understand what is going on in single user mode):

~~~
# ./memory_profiler
              read_memory_loop: 10.68 GiB/s
               read_memory_sse: 13.46 GiB/s
               read_memory_avx: 13.62 GiB/s
             write_memory_loop: 12.84 GiB/s
              write_memory_sse:  8.93 GiB/s
  write_memory_nontemporal_sse: 12.84 GiB/s
              write_memory_avx:  8.91 GiB/s
  write_memory_nontemporal_avx: 12.65 GiB/s
           write_memory_memset: 12.84 GiB/s
~~~

Also, it is
important to realize that `write_memory_loop` gets optimized to a call to
`memset` when reading the numbers above (all other functions are more or less
what you expect):

~~~
0000000100001880 <_write_memory_loop>:
   100001880:   55                      push   %rbp
   100001881:   48 89 e5                mov    %rsp,%rbp
   100001884:   48 89 f0                mov    %rsi,%rax
   100001887:   48 c1 e8 03             shr    $0x3,%rax
   10000188b:   48 85 c0                test   %rax,%rax
   10000188e:   74 13                   je     1000018a3 <_write_memory_loop+0x23>
   100001890:   48 c1 e0 03             shl    $0x3,%rax
   100001894:   48 8d 35 85 03 00 00    lea    0x385(%rip),%rsi        # 100001c20 <_.memset_pattern>
   10000189b:   48 89 c2                mov    %rax,%rdx
   10000189e:   e8 43 02 00 00          callq  100001ae6 <_memset_pattern16$stub>
   1000018a3:   5d                      pop    %rbp
   1000018a4:   c3                      retq
~~~
