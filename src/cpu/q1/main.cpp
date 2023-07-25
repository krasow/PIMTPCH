// main.cpp for cpu test
#include <q1.h>
#include <omp.h>
#include <common/hmap.h>
#include <common/timer.h>

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
	// setup hash table
	struct tpch_hashtable *out = NULL;

	for (uint32_t i = 0; i < tests; i++) {
		// run and time naive
		cpu_t_naive.start();
		q1_naive(&out, l_tups);
		cpu_t_naive.stop();

		struct tpch_hashtable_iter *iter;
		printf("ITERATION %d -----\n", i);
	
		for(iter = tpch_create_htable_iter (out); iter->entry; tpch_htable_iter_advance(iter)) 
		{
			uint16_t key = (uint16_t)tpch_htable_get_iter_key(iter);
			values *val = (values*)tpch_htable_get_iter_value(iter);
			
			printf("%d %d |", ((key>>8) & 0x00ff), key & 0x00ff);
			for (unsigned int j=0; j<val->double_cnt; j++){
				printf("%lu | ", val->doubles[j]);
			} 
			for (unsigned int j=0; j<val->int_cnt; j++){
				printf("%lu | ", val->ints[j]);
			} 
			printf("\n");
		}
		// free hash table
		tpch_free_htable(out, 0, 0);
	}
	cpu_t_naive.print("CPU NAIVE");



	// uint64_t tpch_q1_sum = 0;
	// for (uint32_t i = 0; i < tests; i++) {
	// 	// run and time optimized
	// 	cpu_t.start();
	// 	tpch_q1_sum = q1(l_tups);
	// 	cpu_t.stop();
	// }
	// cpu_t.print("CPU VECTORIZED OPENMP", tpch_q1_sum);

	table_free(&l_tups);
	return 0;
}