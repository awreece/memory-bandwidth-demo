Memory Bandwidth Demo
=====================

I've been attempting to achieve the theoretical best memory bandwidth for reads 
and writes on my machine. For a [Retina Macbook Pro](http://ark.intel.com/products/64891/Intel-Core-i7-3720QM-Processor-6M-Cache-up-to-3_60-GHz), 
I expect 25.6 GB/s. 

Instead, I get:
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

I don't get anywhere close to the full bandwidth, even on reads.
