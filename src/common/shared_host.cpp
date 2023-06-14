#include <iostream>
#include <sys/time.h>
#include <unistd.h>

#include "q6_test.h"
#include "timer.h"

data* retrieve() {
	// input size 
	const uint64_t total_size = NUM_TUPLES * sizeof(data);
	data* tups = (data*)malloc(total_size);
	if (!tups) exit(-1);

#ifdef DEBUG
	printf("tups %p with size %ld and data size %ld -> %p\n",
		tups, total_size, sizeof(data), (void*)(((uint64_t)tups) + total_size));
#endif


	if(access(DATABASE, F_OK)) {
		std::cerr << "Error: no database file found. Must call 'sh q6.sh -t data'" << std::endl;
		exit(1);
	} 
	FILE *f = fopen(DATABASE, "rb");
	if (!fread(tups, sizeof(data), NUM_TUPLES, f)) {
		std::cerr << "Error: mismatching tuples in database file. Must call 'sh q6.sh -t data'" << std::endl;
		exit(1);
	}
	fclose(f);
	
	return tups;
}


void print_data(data* lineitem) {
	for (uint32_t i = 0; i < NUM_TUPLES; i++) {
		std::cout << (lineitem + i)->l_shipdate << ", " <<
			(lineitem + i)->l_discount 			<< ", " <<
			(lineitem + i)->l_quantity 			<< ", " <<
			(lineitem + i)->l_extendedprice <<  std::endl;
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
	std::cout << test << " q6 with " << NUM_TUPLES << " tuples with " 
			  << this->iterations << " iterations: " << std::endl;
	if (output != 0) std::cout << "test result: " << output << std::endl;
	std::cout << "avg time: " << this->average_time << " ms\n" << std::endl;
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