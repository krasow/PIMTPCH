#include <iostream>
#include <unistd.h>

#include "../tpch.h"
#include "lineitem.h"

void retrieve(lineitem** l_tups) {
	const uint64_t total_size = NUM_TUPLES * sizeof(lineitem);
#ifdef __ROW
	* l_tups = (lineitem*)malloc(total_size);
	void* tuple_data = (void*)(*l_tups); // used in fread 
	if (!tuple_data) exit(-1);
#endif
#ifdef __COL
	// allocate data structure that manages the tuples
	* l_tups = (lineitem*)malloc(sizeof(lineitem));
	// allocate data for storing the tuples
	char* tuple_data = (char*)malloc(total_size); // used in fread 
	if (!tuple_data) exit(-1);

	// hacky way to set tuple data to the l_tups data structure
	(*l_tups)->data = tuple_data;
	(*l_tups)->l_shipdate = (uint32_t*)tuple_data;
	uint32_t total_heap_loc = NUM_TUPLES * sizeof((*l_tups)->l_shipdate[0]);
	(*l_tups)->l_discount = (uint64_t*)(tuple_data + total_heap_loc);
	total_heap_loc += NUM_TUPLES * sizeof((*l_tups)->l_discount[0]);
	(*l_tups)->l_quantity = (uint64_t*)(tuple_data + total_heap_loc);
	total_heap_loc += NUM_TUPLES * sizeof((*l_tups)->l_quantity[0]);
	(*l_tups)->l_extendedprice = (uint64_t*)(tuple_data + total_heap_loc);
#endif

#ifdef DEBUG
	printf("l_tups %p with size %ld and data size %ld -> %p\n",
		*l_tups, total_size, sizeof(lineitem), (void*)(((uint64_t)*l_tups) + total_size));
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
}


void print_data(lineitem* l_tups) {
	for (uint32_t i = 0; i < NUM_TUPLES; i++) {
#ifdef __ROW
		std::cout << (l_tups + i)->l_shipdate << ", " <<
			(l_tups + i)->l_discount << ", " <<
			(l_tups + i)->l_quantity << ", " <<
			(l_tups + i)->l_extendedprice << std::endl;
#endif

#ifdef __COL
		std::cout << l_tups->l_shipdate[i] << ", " <<
			l_tups->l_discount[i] << ", " <<
			l_tups->l_quantity[i] << ", " <<
			l_tups->l_extendedprice[i] << std::endl;
#endif
	}
}