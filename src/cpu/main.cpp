// main.cpp for cpu test
#include "q6_cpu.h"
#include "../common/timer.h"


int main(int argc, char *argv[]) {
	srand(SEED);

	// storing times
	class calc_time cpu_t;

	// get from DATABASE stored
	data* tups = retrieve();

#ifdef DEBUG
	print_data(tups);
#endif


	uint64_t tpch_q6_sum = 0;
	uint32_t tests = (argc == 1) ? 1 : atoi(argv[1]);
	
	for (uint32_t i = 0; i < tests; i++) {
		cpu_t.start();
		tpch_q6_sum = tpch_q6(tups);
		cpu_t.stop();
	}

	cpu_t.print("CPU GOLDEN CASE", tpch_q6_sum);

	
	free(tups);
	return 0;
}