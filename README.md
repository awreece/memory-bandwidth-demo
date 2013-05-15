Memory Bandwidth Demo
=====================

I've been attempting to achieve the theoretical best memory bandwidth for reads 
and writes on my machine. For a [Retina Macbook Pro](http://ark.intel.com/products/64891/Intel-Core-i7-3720QM-Processor-6M-Cache-up-to-3_60-GHz), 
I expect 25.6 GB/s. 

Instead, I get:
~~~
$ ./memory_profiler
              read_memory_loop:  3.18 GiB/s
               read_memory_sse:  6.49 GiB/s
               read_memory_avx:  8.62 GiB/s
             write_memory_loop:  3.22 GiB/s
              write_memory_sse:  3.04 GiB/s
  write_memory_nontemporal_sse:  3.28 GiB/s
              write_memory_avx:  6.05 GiB/s
  write_memory_nontemporal_avx: 10.90 GiB/s
           write_memory_memset: 17.02 GiB/s
~~~

I don't get anywhere close to the full bandwidth, even on reads. I think I
have a bug. :(
