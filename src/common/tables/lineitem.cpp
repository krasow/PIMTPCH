#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "lineitem.h"
#include "tables.h"

void retrieve(lineitem** l_tups) {
	const uint64_t total_size = NUM_TUPLES * sizeof(lineitem);
	std::string db = DATABASE;
	db += "/lineitem.tbl";

#ifdef __ROW
	std::string db_bin = db + ".row.bin";
	* l_tups = (lineitem*)malloc(total_size);
	void* tuple_data = (void*)(*l_tups); // used in fread 
	if (!tuple_data) exit(-1);
#endif
#ifdef __COL
	std::string db_bin = db + ".bin";
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

	// check if there has been a binary file already produced from .tbl
	// only passes if table was loaded to the system before
	if (!access(db_bin.c_str(), F_OK)) {
		std::cout << "Found a binary .tbl file: " << db_bin << std::endl;
		FILE* f_bin = fopen(db_bin.c_str(), "rb");

		if (!fread((void*)tuple_data, TUPLE_SIZE, NUM_TUPLES, f_bin)) {
			std::cerr << "Error: mismatching tuples in database file." << std::endl;
			exit(12);
		}
		fclose(f_bin);
		return;
	}

	if (access(db.c_str(), F_OK)) {
		std::cerr << "Error: no .tbl database folder found." << std::endl;
		exit(12);
	}

	// hold an entire row from fgets
	char buffer[1000];
	// hold an entire row separated by pipe delimiter & heap allocation
	char* elems[LINEITEM_COLUMNS - 1];
	for (uint16_t i = 0; i < LINEITEM_COLUMNS; i++) {
		elems[i] = (char*)malloc(128);
	}

	uint64_t curr_tuple = 0;
	FILE* f = fopen(db.c_str(), "rb");
	// goes thru all the tuples
	while (fgets(buffer, sizeof(buffer), f) && curr_tuple < NUM_TUPLES) {
		uint16_t curr_elem = 0;
		int bytes = 0;
		// store one column from the buffer ("STORE THIS"|"NOT THIS"|)
		char tmp[128];
		// used to increment the buffer
		char* loc_buffer = buffer;

		// for each element in each tuple
		while (sscanf(loc_buffer, "%[^`|`]|%n", tmp, &bytes) == 1 && curr_elem < LINEITEM_COLUMNS) {
			strncpy(elems[curr_elem], tmp, bytes);
			curr_elem++;
			loc_buffer += bytes;
		}
#ifdef __ROW
		(*l_tups)[curr_tuple].l_shipdate = convert_date(elems[10]);
		(*l_tups)[curr_tuple].l_discount = (uint64_t)ceil(strtod(elems[6], &elems[6 + 1]) * 100); // double to int conversion
		(*l_tups)[curr_tuple].l_quantity = (uint64_t)(atoi(elems[3]));
		(*l_tups)[curr_tuple].l_extendedprice = (uint64_t)ceil(strtod(elems[5], &elems[5 + 1]));
#endif
#ifdef __COL
		(*l_tups)->l_shipdate[curr_tuple] = convert_date(elems[10]);
		(*l_tups)->l_discount[curr_tuple] = (uint64_t)ceil(strtod(elems[6], &elems[6 + 1]) * 100); // double to int conversion
		(*l_tups)->l_quantity[curr_tuple] = (uint64_t)(atoi(elems[3]));
		(*l_tups)->l_extendedprice[curr_tuple] = (uint64_t)ceil(strtod(elems[5], &elems[5 + 1]));
#endif
		curr_tuple++;
	}
	fclose(f);

	// write the table to a binary file to avoid conversions
	FILE* f_write = fopen(db_bin.c_str(), "wb");
	fwrite((void*)tuple_data, TUPLE_SIZE, curr_tuple, f_write);
	printf("total tuples : %lu\n", curr_tuple);
	fclose(f_write);
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