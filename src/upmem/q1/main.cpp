// tpch_q1_test_host
#include <stdio.h>
#include <assert.h>
#include <string>
#include <iostream>

#include <pim.h>
#include <q1.h>

#include <common/timer.h>
#include <common/hmap.h>

// dpu specific 
#include <dpu>


int main(int argc, char* argv[]) {
	srand(SEED);

	// storing times
	class calc_time dpu_t, dpu_transfer_t, dpu_reduction_t;

	uint32_t tests = (argc == 1) ? 1 : atoi(argv[1]);

	// get from DATABASE stored
	lineitem* l_tups = NULL;
	retrieve(&l_tups);

	// init output
	uint64_t q1_out = 0;

#ifdef DEBUG
	print_data(l_tups);
#endif

	// START UPMEM CODE 
	// prepare structs for dpu sets
	struct dpu_set_t dpu_set, dpu_reduce, dpu;
	uint32_t nr_of_dpus;

	// allocate and load DPU binaries
	DPU_ASSERT(dpu_alloc(NR_DPUS, "backend=simulator", &dpu_set));
	DPU_ASSERT(dpu_load(dpu_set, Q1_DPU_BINARY, NULL));

	DPU_ASSERT(dpu_alloc(1, "backend=simulator", &dpu_reduce));
	DPU_ASSERT(dpu_load(dpu_reduce, DPU_REDUCE_BINARY, NULL));

	for (uint32_t i = 0; i < tests; i++) {
		DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));

		// number of tuples per DPU
		const uint64_t tuples_per_dpu = divceil(l_tups->elements, nr_of_dpus);

		// bytes per DPU 
		const uint64_t total_mram_size = tuples_per_dpu * TUPLE_SIZE;


		// Input arguments
		dpu_arguments_t input_arguments[nr_of_dpus];
		for (uint32_t i = 0; i < nr_of_dpus - 1; i++) {
			input_arguments[i].size = total_mram_size;
			input_arguments[i].transfer_size = total_mram_size;
		}
		input_arguments[nr_of_dpus - 1].size = (l_tups->elements * TUPLE_SIZE - total_mram_size * (nr_of_dpus - 1));
		input_arguments[nr_of_dpus - 1].transfer_size = total_mram_size;


		uint32_t idx_dpu = 0;


#ifndef SIMULATOR // method is not accurate in simulator
		// start transfer time
		dpu_transfer_t.start();
#endif

		// transfer input arguments 
		DPU_FOREACH(dpu_set, dpu, idx_dpu) {
			DPU_ASSERT(dpu_prepare_xfer(dpu, &input_arguments[idx_dpu]));
		}
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments[0]), DPU_XFER_DEFAULT));


		idx_dpu = 0; //reset idx



#ifdef __COL
		// transfer input table  
		DPU_FOREACH(dpu_set, dpu, idx_dpu) {
			void* temp_loc = &l_tups->l_shipdate[tuples_per_dpu * idx_dpu];
			DPU_ASSERT(dpu_prepare_xfer(dpu, temp_loc));
		}
		uint32_t txf_size = tuples_per_dpu * sizeof(l_tups->l_shipdate[0]);
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, txf_size, DPU_XFER_DEFAULT));
		uint32_t total_heap_loc = txf_size;

		DPU_FOREACH(dpu_set, dpu, idx_dpu) {
			void* temp_loc = &l_tups->l_discount[tuples_per_dpu * idx_dpu];
			DPU_ASSERT(dpu_prepare_xfer(dpu, temp_loc));
		}
		txf_size = tuples_per_dpu * sizeof(l_tups->l_discount[0]);
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, total_heap_loc, txf_size, DPU_XFER_DEFAULT));
		total_heap_loc += txf_size;

		DPU_FOREACH(dpu_set, dpu, idx_dpu) {
			void* temp_loc = &l_tups->l_quantity[tuples_per_dpu * idx_dpu];
			DPU_ASSERT(dpu_prepare_xfer(dpu, temp_loc));
		}
		txf_size = tuples_per_dpu * sizeof(l_tups->l_quantity[0]);
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, total_heap_loc, txf_size, DPU_XFER_DEFAULT));
		total_heap_loc += txf_size;


		DPU_FOREACH(dpu_set, dpu, idx_dpu) {
			void* temp_loc = &l_tups->l_extendedprice[tuples_per_dpu * idx_dpu];
			DPU_ASSERT(dpu_prepare_xfer(dpu, temp_loc));
		}
		txf_size = tuples_per_dpu * sizeof(l_tups->l_extendedprice[0]);
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, total_heap_loc, txf_size, DPU_XFER_DEFAULT));
		total_heap_loc += txf_size;

		assert(total_heap_loc == total_mram_size);
#endif






#ifndef SIMULATOR
		dpu_transfer_t.stop(); // not accurate in simulation
#endif



#ifndef SIMULATOR // method is not accurate in simulator
		// start DPU execution time
		dpu_t.start();
#endif

		// launch DPUs in synchronous mode (wait until complete)
		DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));


#ifndef SIMULATOR
		// end DPU execution time
		dpu_t.stop();
#endif

#ifdef SIMULATOR // timing information in simulation
		// retrieve number of instructions
		uint32_t nb_perf;
		uint32_t nb_perf_total = 0;
		DPU_FOREACH(dpu_set, dpu) {
			DPU_ASSERT(dpu_copy_from(dpu, "nb_perf", 0, &nb_perf, sizeof(uint32_t)));
			nb_perf_total += nb_perf;
		}

		// retrieve DPU frequency
		uint32_t clocks_per_sec;
		DPU_FOREACH(dpu_set, dpu) {
			DPU_ASSERT(dpu_copy_from(dpu, "CLOCKS_PER_SEC", 0, &clocks_per_sec, sizeof(uint32_t)));
		}
		// assume 1 insturction per clock on average
		double dpu_time = ((double)(nb_perf_total / nr_of_dpus) / clocks_per_sec) * 1000;
		// manual set for the average time
		dpu_t.set(dpu_time);
#endif 

#ifdef PRINT
		DPU_FOREACH(dpu_set, dpu) {
			DPU_ASSERT(dpu_log_read(dpu, stdout));
		}
#endif

		// PARALLEL RETRIEVE TRANSFER
		idx_dpu = 0; // reset idx 

		// set output
		uint32_t dpu_output_size = sizeof(uint64_t) * NR_TASKLETS;
		uint32_t results_size = dpu_output_size * nr_of_dpus;

		uint64_t* results = (uint64_t*)malloc(results_size);
		memset(results, 0, results_size);

		DPU_FOREACH(dpu_set, dpu, idx_dpu) {
			DPU_ASSERT(dpu_prepare_xfer(dpu, &results[idx_dpu * NR_TASKLETS]));
		}

		// total_mram_size is used as the offset for storing return in MRAM
		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, total_mram_size, dpu_output_size, DPU_XFER_DEFAULT));


// #ifdef PRINT
// 		std::cout << "Transfer results for each DPU" << std::endl;
// 		uint16_t dpu_count = 0;
// 		for (uint32_t j = 0; j < nr_of_dpus * NR_TASKLETS; j++) {
// 			std::cout << "DPU " << dpu_count << " tasklet "
// 				<< j % NR_TASKLETS << " \t--> " << results[j] << std::endl;
// 			if ((j % NR_TASKLETS == 0) && (j >= NR_TASKLETS)) { dpu_count++; }
// 		}
// #endif


		// REDUCE KERNEL
		DPU_ASSERT(dpu_get_nr_dpus(dpu_reduce, &nr_of_dpus));

		dpu_arguments_t reduce_args;
		reduce_args.size = results_size;
		reduce_args.transfer_size = reduce_args.size;
		// transfer args
		DPU_FOREACH(dpu_reduce, dpu) {
			DPU_ASSERT(dpu_prepare_xfer(dpu, &reduce_args));
		}
		DPU_ASSERT(dpu_push_xfer(dpu_reduce, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments[0]), DPU_XFER_DEFAULT));

		DPU_FOREACH(dpu_reduce, dpu) {
			DPU_ASSERT(dpu_prepare_xfer(dpu, results));
		}
		DPU_ASSERT(dpu_push_xfer(dpu_reduce, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, reduce_args.transfer_size, DPU_XFER_DEFAULT));

#ifndef SIMULATOR // method is not accurate in simulator
		// start DPU execution time
		dpu_reduction_t.start();
#endif

		// launch DPUs in synchronous mode (wait until complete)
		DPU_ASSERT(dpu_launch(dpu_reduce, DPU_SYNCHRONOUS));

#ifndef SIMULATOR
		dpu_reduction_t.stop();
#endif
		// set output
		q1_out = 0;
		dpu_output_size = sizeof(uint64_t);
		DPU_FOREACH(dpu_reduce, dpu) {
			DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, reduce_args.transfer_size, &q1_out, dpu_output_size));
		}
#ifdef SIMULATOR // timing information in simulation
		// retrieve number of instructions
		uint32_t red_nb_perf = 0;
		DPU_FOREACH(dpu_reduce, dpu) {
			DPU_ASSERT(dpu_copy_from(dpu, "nb_perf", 0, &red_nb_perf, sizeof(uint32_t)));
		}
		// retrieve DPU frequency
		DPU_FOREACH(dpu_reduce, dpu) {
			DPU_ASSERT(dpu_copy_from(dpu, "CLOCKS_PER_SEC", 0, &clocks_per_sec, sizeof(uint32_t)));
		}
		// assume 1 insturction per clock on averreage
		double red_time = ((double)red_nb_perf / clocks_per_sec) * 1000;
		// manual set for the average time
		dpu_reduction_t.set(red_time);
#endif
#ifdef PRINT
		DPU_FOREACH(dpu_reduce, dpu) {
			DPU_ASSERT(dpu_log_read(dpu, stdout));
		}
		std::cout << "Result from reduction: " << q1_out << std::endl;
#endif 

		free(results);
	}


	dpu_transfer_t.print("DPU TRANSFER TEST");
	dpu_t.print("DPU EXECUTION TIME");
	dpu_reduction_t.print("DPU REDUCTION TIME", q1_out);

	double total_dpu_time = dpu_transfer_t.time() + dpu_t.time() + dpu_reduction_t.time();

	printf("Total DPU time: %f\n", total_dpu_time);

	
	table_free(&l_tups);

	DPU_ASSERT(dpu_free(dpu_set));
	DPU_ASSERT(dpu_free(dpu_reduce));
	return 0;
}