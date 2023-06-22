// main.cpp for cpu test
#include <stdio.h>
#include "../common/q6_test.h"
#include "../common/timer.h"



void fill(data* lineitem)
{
	int YYYY = 0;
	int MM = 0;
	int DD = 0;

	for (uint32_t i = 0; i < NUM_TUPLES; i++) {
		YYYY = rand() % 70 + 1970; // 1970-2040
		MM = rand() % 12 + 1; 	 // 1-12
		DD = rand() % 31 + 1; 	// 1-31

		struct tm ti = { 0 };
		ti.tm_year = YYYY;
		ti.tm_mon = MM;
		ti.tm_mday = DD;

#ifdef __ROW
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
#endif

#ifdef __COL
		lineitem->l_shipdate[i] = (uint32_t)mktime(&ti);
		lineitem->l_discount[i] = rand() % 100; 	// 0-100
		lineitem->l_quantity[i] = rand() % 100 + 1; // 1-100
		lineitem->l_extendedprice[i] = ((double)rand() / RAND_MAX) * (double)100; // 0-100
#endif
	}
}



int main(int argc, char* argv[]) {
	srand(SEED);
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
	printf("tups %p with size %ld and tup size %ld -> %p\n",
		tups, total_size, TUPLE_SIZE, (void*)(((uint64_t)tups) + total_size));
#endif

	// fill table with random data
	fill(tups);

#ifdef DEBUG
	print_data(tups);
#endif

	FILE* f = fopen(DATABASE, "wb");
	fwrite(tuple_data, TUPLE_SIZE, NUM_TUPLES, f);
	fclose(f);
#ifdef __COL
	free(tups->data);
#endif
	free(tups);
	return 0;
}