// main.cpp for cpu test
#include "q6_cpu.h"

#include <omp.h>


int main(int argc, char* argv[]) {
	srand(SEED);

	// storing times
	class calc_time cpu_t_naive, cpu_t;
	
	uint32_t tests = (argc == 1) ? 1 : atoi(argv[1]);
	// get from DATABASE stored
	data* tups = retrieve();

#ifdef DEBUG
	// print_data(tups);
	tpch_selectivity_print(tups);
#endif

	tpch_selectivity_print(tups);
	
	uint64_t naive_tpch_q6_sum = 0;
	for (uint32_t i = 0; i < tests; i++) {
		// run and time naive
		cpu_t_naive.start();
		naive_tpch_q6_sum = tpch_q6_naive(tups);
		cpu_t_naive.stop();
	}
	cpu_t_naive.print("CPU NAIVE", naive_tpch_q6_sum);



	uint64_t tpch_q6_sum = 0;
	for (uint32_t i = 0; i < tests; i++) {
		// run and time optimized
		cpu_t.start();
		tpch_q6_sum = tpch_q6(tups);
		cpu_t.stop();
	}
	cpu_t.print("CPU VECTORIZED OPENMP", tpch_q6_sum);



	// free database
#ifdef __COL
	free(tups->data);
#endif
	free(tups);

	return 0;
}