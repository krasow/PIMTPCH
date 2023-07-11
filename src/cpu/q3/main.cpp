// main.cpp for cpu test
#include <q3_cpu.h>
#include <omp.h>

int main(int argc, char* argv[]) {
	srand(SEED);

	// storing times
	class calc_time cpu_t_naive, cpu_t;
	
	uint32_t tests = (argc == 1) ? 1 : atoi(argv[1]);
	// get from DATABASE stored
	lineitem* 	l_tups = NULL;
	orders* 	o_tups = NULL;
	customer* 	c_tups = NULL;
	retrieve(&l_tups);
	retrieve(&o_tups);
	retrieve(&c_tups);

#ifdef DEBUG
	print_data(l_tups);
	print_data(o_tups);
	print_data(c_tups);
#endif

	// print_data(c_tups);
	uint64_t naive_tpch_q3_sum = 0;
	for (uint32_t i = 0; i < tests; i++) {
		// run and time naive
		cpu_t_naive.start();
		naive_tpch_q3_sum = q3_naive(l_tups, o_tups, c_tups);
		cpu_t_naive.stop();
	}
	cpu_t_naive.print("CPU NAIVE", naive_tpch_q3_sum);



	// uint64_t tpch_q3_sum = 0;
	// for (uint32_t i = 0; i < tests; i++) {
	// 	// run and time optimized
	// 	cpu_t.start();
	// 	tpch_q3_sum = q3(l_tups, o_tups, c_tups);
	// 	cpu_t.stop();
	// }
	// cpu_t.print("CPU VECTORIZED OPENMP", tpch_q3_sum);

	table_free(&l_tups);
	table_free(&o_tups);

	return 0;
}