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
#ifdef __ROW
	const uint64_t total_size = NUM_TUPLES * sizeof(data);
	const uint64_t elems = NUM_TUPLES;
#endif
#ifdef __COL
	const uint64_t total_size = sizeof(data);
	const uint64_t elems = 1;
#endif
	srand(SEED);
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

	FILE* f = fopen(DATABASE, "wb");
	fwrite(tups, sizeof(data), elems, f);
	fclose(f);

	free(tups);
	return 0;
}