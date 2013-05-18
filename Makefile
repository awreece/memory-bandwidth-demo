memory_profiler: main.c monotonic_timer.c functions.c
	gcc -O3 -march=native $^ -o $@ -fopenmp -DWITH_OPENMP

.PHONY: run
run: memory_profiler
	./memory_profiler
