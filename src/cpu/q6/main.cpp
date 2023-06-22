// main.cpp for cpu test
#include "q6_cpu.h"
#include <omp.h>

int main(int argc, char* argv[]) {
	srand(SEED);

	// storing times
	class calc_time cpu_t_naive, cpu_t;
	
	uint32_t tests = (argc == 1) ? 1 : atoi(argv[1]);
	// get from DATABASE stored
	lineitem* l_tups = NULL;
	retrieve(&l_tups);

#ifdef DEBUG
	print_data(l_tups);
#endif

	q6_selectivity_print(l_tups);
	
	uint64_t naive_tpch_q6_sum = 0;
	for (uint32_t i = 0; i < tests; i++) {
		// run and time naive
		cpu_t_naive.start();
		naive_tpch_q6_sum = q6_naive(l_tups);
		cpu_t_naive.stop();
	}
	cpu_t_naive.print("CPU NAIVE", naive_tpch_q6_sum);



	uint64_t tpch_q6_sum = 0;
	for (uint32_t i = 0; i < tests; i++) {
		// run and time optimized
		cpu_t.start();
		tpch_q6_sum = q6(l_tups);
		cpu_t.stop();
	}
	cpu_t.print("CPU VECTORIZED OPENMP", tpch_q6_sum);



	// free database
#ifdef __COL
	free(l_tups->data);
#endif
	free(l_tups);

	return 0;
}