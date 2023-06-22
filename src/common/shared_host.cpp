#include <iostream>
#include <sys/time.h>
#include <unistd.h>

#include "q6_test.h"
#include "timer.h"

data* retrieve() {
	const uint64_t total_size = NUM_TUPLES * sizeof(data);
#ifdef __ROW
	data* tups = (data*)malloc(total_size);
	void* tuple_data = (void*)tups; // used in fread 
	if (!tups) exit(-1);
#endif
#ifdef __COL
	// allocate data structure that manages the tuples
	data* tups = (data*)malloc(sizeof(data));
	// allocate data for storing the tuples
	char* tuple_data = (char*)malloc(total_size); // used in fread 
	if (!tuple_data) exit(-1);

	// hacky way to set tuple data to the tups data structure
	tups->data = tuple_data;
	tups->l_shipdate = (uint32_t*)tuple_data;
	uint32_t total_heap_loc = NUM_TUPLES * sizeof(tups->l_shipdate[0]);
	tups->l_discount = (uint64_t*)(tuple_data + total_heap_loc);
	total_heap_loc += NUM_TUPLES * sizeof(tups->l_discount[0]);
	tups->l_quantity = (uint64_t*)(tuple_data + total_heap_loc);
	total_heap_loc += NUM_TUPLES * sizeof(tups->l_quantity[0]);
	tups->l_extendedprice = (uint64_t*)(tuple_data + total_heap_loc);
#endif

#ifdef DEBUG
	printf("tups %p with size %ld and data size %ld -> %p\n",
		tups, total_size, sizeof(data), (void*)(((uint64_t)tups) + total_size));
#endif


	if (access(DATABASE, F_OK)) {
		std::cerr << "Error: no database file found. Must call 'sh q6.sh -t data'" << std::endl;
		exit(12);
	}
	FILE* f = fopen(DATABASE, "rb");
	if (!fread((void*)tuple_data, TUPLE_SIZE, NUM_TUPLES, f)) {
		std::cerr << "Error: mismatching tuples in database file. Must call 'sh q6.sh -t data'" << std::endl;
		exit(12);
	}
	fclose(f);

	return tups;
}


void print_data(data* lineitem) {
	for (uint32_t i = 0; i < NUM_TUPLES; i++) {
#ifdef __ROW
		std::cout << (lineitem + i)->l_shipdate << ", " <<
			(lineitem + i)->l_discount << ", " <<
			(lineitem + i)->l_quantity << ", " <<
			(lineitem + i)->l_extendedprice << std::endl;
#endif

#ifdef __COL
		std::cout << lineitem->l_shipdate[i] << ", " <<
			lineitem->l_discount[i] << ", " <<
			lineitem->l_quantity[i] << ", " <<
			lineitem->l_extendedprice[i] << std::endl;
#endif
	}
}


void calc_time::start() {
	gettimeofday(&startTime, NULL);
}

void calc_time::stop() {
	gettimeofday(&stopTime, NULL);
	this->update();
}

void calc_time::set(double time) {
	this->iterations++;
	this->total_time += time;
	this->average_time = this->total_time / this->iterations;
}

void calc_time::print(std::string test, uint64_t output) {
	printf("%s q6 with %d tuples with %d iterations\n", test.c_str(), NUM_TUPLES, this->iterations);
	if (output != 0)  printf("test result: %lu\n", output);
	printf("avg time: %f ms\n\n", this->average_time);
}

void calc_time::print(std::string test) {
	this->print(test, 0);
}

void calc_time::update() {
	this->iterations++;
	this->total_time += ((stopTime.tv_sec - startTime.tv_sec) * 1000000.0 +
		stopTime.tv_usec - startTime.tv_usec) / 1000;

	this->average_time = this->total_time / this->iterations;
}

double calc_time::time() {
	return this->average_time;
}