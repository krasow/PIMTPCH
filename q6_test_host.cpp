// tpch_q6_test_host
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

#include <string>
#include <iostream>

#include "q6_test.h"
#include "q6_test_cpu.h"

// dpu specific 
#include <dpu>

#ifndef DPU_BINARY
#define DPU_BINARY "./q6_dpu"
#define DPU_REDUCE_BINARY "./q6_reduce"
#endif

// using namespace dpu;


void fill(data* lineitem)
{
	int YYYY = 0;
	int MM = 0;
	int DD = 0;

	for (uint32_t i = 0; i < NUM_TUPLES; i++) {
		YYYY = rand() % 70 + 1970; // 1970-2040
		MM = rand() % 12 + 1; 	 // 1-12
		DD = rand() % 31 + 1; 	// 1-31

		struct tm ti={0};
		ti.tm_year = YYYY;
		ti.tm_mon  = MM;
		ti.tm_mday = DD;
	
		(lineitem + i)->l_shipdate = (uint32_t)mktime(&ti);

		// char MM_str[3]; sprintf(MM_str, "%02d", MM & 0x1F);
		// char DD_str[3]; sprintf(DD_str, "%02d", DD & 0x1F);

		// std::string year = std::to_string(YYYY)
		// 	+ "-" + MM_str
		// 	+ "-" + DD_str;


		// for (uint32_t j = 0; j < DATE_LENGTH; j++) {
		// 	(lineitem + i)->l_shipdate[j] = year[j];
		// }

		// (lineitem + i)->l_discount = (double)rand() / RAND_MAX; // 0-1
		(lineitem + i)->l_discount = rand() % 100; 	   // 0-100
		(lineitem + i)->l_quantity = rand() % 100 + 1; // 1-100
		(lineitem + i)->l_extendedprice = ((double)rand() / RAND_MAX) * (double)100; // 0-100
	}
}


void print_data(data* lineitem) {
	for (uint32_t i = 0; i < NUM_TUPLES; i++) {
		std::cout << (lineitem + i)->l_shipdate << ", " <<
			(lineitem + i)->l_discount 			<< ", " <<
			(lineitem + i)->l_quantity 			<< ", " <<
			(lineitem + i)->l_extendedprice <<  std::endl;
	}
}


class calc_time {
private:
	struct timeval startTime;
	struct timeval stopTime;
	double time_stamp;

public:
	void start() {
		gettimeofday(&startTime, NULL);
	}

	void stop() {
		gettimeofday(&stopTime, NULL);
	}

	void print(std::string test, uint64_t output) {
		this->time();

		std::cout << test << " q6 with " << NUM_TUPLES << " tuples: " << std::endl;
		if (output != 0) std::cout << "test result: " << output << std::endl;
		std::cout << "time: " << this->time_stamp << " ms\n" << std::endl;
	}

	void print(std::string test) {
		print(test, 0);
	}

	double time() {
		this->time_stamp = ((stopTime.tv_sec - startTime.tv_sec) * 1000000.0 +
		stopTime.tv_usec - startTime.tv_usec) / 1000;
		return this->time_stamp;
	}
};

int main(void) {
	// srand(time(NULL)); 	// non-repro
	srand(72); 			// repro

	// storing times
	class calc_time cpu_t, dpu_t, dpu_transfer_t, dpu_reduction_t;

	// input size 
	const uint64_t total_size = NUM_TUPLES * sizeof(data);
	data* tups = (data*)malloc(total_size);
	if (!tups) exit(-1);

#ifdef DEBUG
	printf("tups %p with size %ld and data size %ld -> %p\n",
		tups, total_size, sizeof(data), (void*)(((uint64_t)tups) + total_size));
#endif

	// fill table with random data
	fill(tups);

#ifdef DEBUG
	print_data(tups);
#endif

	uint64_t tpch_q6_sum = 0;

	cpu_t.start();
	tpch_q6_sum = tpch_q6(tups);
	cpu_t.stop();
	cpu_t.print("CPU GOLDEN CASE", tpch_q6_sum);


	// START UPMEM CODE 
	struct dpu_set_t dpu_set, dpu;
	uint32_t nr_of_dpus;

	// allocate and load DPU binaries
	DPU_ASSERT(dpu_alloc(NUM_DPUS, "backend=simulator", &dpu_set));
	DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));
	DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));

	printf("Allocated %d DPU(s)\n", nr_of_dpus);

	// total size 8 byte aligned
	// const uint32_t total_8bytes =
	// 	(total_size % 8) != 0 ? roundup(total_size, 8) : total_size;

	// number of tuples per DPU
	const uint64_t input_dpu_elems = divceil(NUM_TUPLES, nr_of_dpus);

	// bytes per DPU 8 byte aligned 
	const uint64_t input_dpu_8bytes = input_dpu_elems * sizeof(data);

	// const uint64_t input_dpu_8bytes = 
	// 	(input_dpu_bytes % 8) != 0 ? roundup(input_dpu_bytes, 8) : input_dpu_bytes;



	// Input arguments
	dpu_arguments_t input_arguments[nr_of_dpus];
	for (uint32_t i = 0; i < nr_of_dpus - 1; i++) {
		input_arguments[i].size = input_dpu_8bytes;
		input_arguments[i].transfer_size = input_dpu_8bytes;
	}
	input_arguments[nr_of_dpus - 1].size = (NUM_TUPLES * sizeof(data) - input_dpu_8bytes * (nr_of_dpus - 1));
	input_arguments[nr_of_dpus - 1].transfer_size = input_dpu_8bytes;


	uint32_t idx_dpu = 0;

	// start transfer time
	dpu_transfer_t.start();
	// transfer input arguments 
	DPU_FOREACH(dpu_set, dpu, idx_dpu) {
		DPU_ASSERT(dpu_prepare_xfer(dpu, &input_arguments[idx_dpu]));
	}
	DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments[0]), DPU_XFER_DEFAULT));


	idx_dpu = 0; //reset idx

#ifdef DEBUG
	printf("before loop tups %p, i %p\n", tups, &idx_dpu);
#endif

	// transfer input table  
	DPU_FOREACH(dpu_set, dpu, idx_dpu) {
		void* temp_loc = &tups[input_dpu_elems * idx_dpu];
		DPU_ASSERT(dpu_prepare_xfer(dpu, temp_loc));
#ifdef DEBUG
		std::cout << "prepare xfer" << temp_loc << std::endl;
#endif 
	}

#ifdef DEBUG
	std::cout << "push xfer " << DPU_MRAM_HEAP_POINTER_NAME << " " << input_dpu_8bytes << std::endl;
#endif
	// message sizes have to match
	DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, input_dpu_8bytes, DPU_XFER_DEFAULT));
	// end transfer time
	dpu_transfer_t.stop();
	dpu_transfer_t.print("DPU TRANSFER TEST");


	// start DPU execution time
	dpu_t.start();
	// launch DPUs in synchronous mode (wait until complete)
	DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
	// end DPU execution time
	dpu_t.stop();
	dpu_t.print("DPU EXECUTION TIME");
#ifdef PRINT
	DPU_FOREACH(dpu_set, dpu) {
		DPU_ASSERT(dpu_log_read(dpu, stdout));
	}
#endif

	// PARALLEL RETRIEVE TRANSFER
	idx_dpu = 0; // reset idx 

	// set output
	uint32_t dpu_output_size = sizeof(uint64_t) * NUM_TASKLETS;
	uint32_t results_size = dpu_output_size * nr_of_dpus;

	uint64_t* results = (uint64_t*)malloc(results_size);
	memset(results, 0, results_size);

	DPU_FOREACH(dpu_set, dpu, idx_dpu) {
		DPU_ASSERT(dpu_prepare_xfer(dpu, &results[idx_dpu * NUM_TASKLETS]));
	}

	// input_dpu_8bytes is used as the offset for storing return in MRAM
	DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, input_dpu_8bytes, dpu_output_size, DPU_XFER_DEFAULT));


#ifdef PRINT
	std::cout << "Transfer results for each DPU" << std::endl;
	uint16_t dpu_count = 0;
	for (uint32_t j = 0; j < nr_of_dpus * NUM_TASKLETS; j++) {
		std::cout << "DPU " << dpu_count << " tasklet " 
		<< j % NUM_TASKLETS  << " \t--> " << results[j] << std::endl;
		if (j % NUM_TASKLETS == 0) {dpu_count++;}
	}
#endif



	// REDUCE KERNEL

	// DPU_ASSERT(dpu_alloc(1, "backend=simulator", &dpus));
	DPU_ASSERT(dpu_load(dpu, DPU_REDUCE_BINARY, NULL));
	DPU_ASSERT(dpu_get_nr_dpus(dpu, &nr_of_dpus));

	printf("Reduction with %d DPU(s)\n", nr_of_dpus);
	dpu_arguments_t reduce_args;
	reduce_args.size = results_size;
	reduce_args.transfer_size = reduce_args.size;
	// transfer args
	DPU_ASSERT(dpu_prepare_xfer(dpu, &reduce_args));
	DPU_ASSERT(dpu_push_xfer(dpu, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments[0]), DPU_XFER_DEFAULT));

	// set output
	dpu_output_size = sizeof(uint64_t);
	uint64_t q6_out = 0;

	DPU_ASSERT(dpu_prepare_xfer(dpu, results));
	DPU_ASSERT(dpu_push_xfer(dpu, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, reduce_args.transfer_size, DPU_XFER_DEFAULT));
	
	dpu_reduction_t.start();
	DPU_ASSERT(dpu_launch(dpu, DPU_SYNCHRONOUS));
	dpu_reduction_t.stop();
	dpu_reduction_t.print("DPU REDUCTION TIME");

	DPU_ASSERT(dpu_copy_from(dpu, DPU_MRAM_HEAP_POINTER_NAME, reduce_args.transfer_size, &q6_out, dpu_output_size));

#ifdef PRINT
	DPU_ASSERT(dpu_log_read(dpu, stdout));
	std::cout << "Result from reduction: " << q6_out << std::endl;
#endif 

	double total_dpu_time = dpu_transfer_t.time() + 
							dpu_t.time() +
							dpu_reduction_t.time();

	printf("Total DPU time: %f\n", total_dpu_time);

	free(tups);
	free(results);
	DPU_ASSERT(dpu_free(dpu_set));
	return 0;
}