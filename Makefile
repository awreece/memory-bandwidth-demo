memory_profiler: main.c monotonic_timer.c functions.c
	clang -march=native $^ -o $@

.PHONY: run
run: memory_profiler
	./memory_profiler
