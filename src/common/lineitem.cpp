#include <iostream>
#include <string>

#include <unistd.h>
#include <string.h>
#include <math.h>

#include <common/lineitem.h>
#include <common/tables.h>
#include <common/tpch.h>

void allocate(lineitem** l_tups);
void fill(lineitem* l_tups, char** elems, uint64_t curr_tuple);

void retrieve(lineitem** l_tups) {
	std::string db = DATABASE;
	db += "/lineitem.tbl";

#ifdef __ROW
	std::string db_bin = db + ".row.bin";
#endif
#ifdef __COL
	std::string db_bin = db + ".bin";
#endif

	allocate(l_tups);

	// check if there has been a binary file already produced from .tbl
	// only passes if table was loaded to the system before

	if (!access(db_bin.c_str(), F_OK)) {
		std::cout << "Found a binary .tbl file: " << db_bin << std::endl;
		FILE* f_bin = fopen(db_bin.c_str(), "rb");
		fseek(f_bin, 0L, SEEK_END);
		uint64_t bytes_read = ftell(f_bin);
		fseek(f_bin, 0L, SEEK_SET);

		uint32_t tuple_cnt = bytes_read / TUPLE_SIZE;
		(*l_tups)->elements = tuple_cnt;
		td_reallocate(&(*l_tups)->td, tuple_cnt);

		fread((void*)(*l_tups)->l_orderkey, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_partkey, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_suppkey, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_linenumber, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_quantity, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_extendedprice, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_discount, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_tax, sizeof(uint64_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_returnflag, sizeof(uchar_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_linestatus, sizeof(uchar_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_shipdate, sizeof(uint32_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_commitdate, sizeof(uint32_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_receiptdate, sizeof(uint32_t), tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_shipinstruct, (*l_tups)->td.strings.sizes[0], tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_shipmode, (*l_tups)->td.strings.sizes[1], tuple_cnt, f_bin);
		fread((void*)(*l_tups)->l_comment, (*l_tups)->td.strings.sizes[2], tuple_cnt, f_bin);

		fclose(f_bin);

		return;
	}


	if (access(db.c_str(), F_OK)) {
		std::cerr << "Error: no .tbl database folder found." << std::endl;
		exit(12);
	}

	// hold an entire row from fgets
	char buffer[ROW_BUFFER];
	// hold an entire row separated by pipe delimiter & heap allocation
	char **elems = (char**)malloc(LINEITEM_COLUMNS * sizeof(char*));
	
	for (uint16_t i = 0; i < LINEITEM_COLUMNS; i++) {
		elems[i] = (char*)malloc(COLUMN_BUFFER);
	}

	uint64_t curr_tuple = 0;
	FILE* f = fopen(db.c_str(), "rb");
	// goes thru all the tuples
	while (fgets(buffer, sizeof(buffer), f) && curr_tuple < MAX_TUPLES) {
		uint16_t curr_elem = 0;
		int bytes = 0;
		// store one column from the buffer ("STORE THIS"|"NOT THIS"|)
		char tmp[COLUMN_BUFFER];

		// used to increment the buffer
		char* loc_buffer = buffer;
		// for each element in each tuple
		while (curr_elem < LINEITEM_COLUMNS && 
			   sscanf(loc_buffer, "%[^|]|%n", tmp, &bytes) == 1) 
		{
			strncpy(elems[curr_elem], tmp, bytes);
			elems[curr_elem][bytes+1] = '\0'; // add NULL terminating character
			curr_elem++;
			loc_buffer += bytes;
		}


		// fill elements into l_tups data struct
		fill(*l_tups, elems, curr_tuple);
		curr_tuple++;
	}
	fclose(f);

	(*l_tups)->elements = curr_tuple;

	td_reallocate(&(*l_tups)->td, (*l_tups)->elements);

	// write the table to a binary file to avoid conversions

	FILE* f_write = fopen(db_bin.c_str(), "wb");
	fwrite((void*)(*l_tups)->l_orderkey, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_partkey, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_suppkey, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_linenumber, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_quantity, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_extendedprice, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_discount, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_tax, sizeof(uint64_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_returnflag, sizeof(uchar_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_linestatus, sizeof(uchar_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_shipdate, sizeof(uint32_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_commitdate, sizeof(uint32_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_receiptdate, sizeof(uint32_t), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_shipinstruct, GET_STRING_SIZE((*l_tups), 0), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_shipmode, GET_STRING_SIZE((*l_tups), 1), curr_tuple, f_write);
	fwrite((void*)(*l_tups)->l_comment, GET_STRING_SIZE((*l_tups), 2), curr_tuple, f_write);

	printf("total tuples : %lu\n", curr_tuple);
	fclose(f_write);
}



#ifdef __ROW
void allocate(lineitem** l_tups) {
	*l_tups = (lineitem*)malloc(sizeof(lineitem));
	void* tuple_data = (void*)malloc(total_size); // used in fread 
	if (!tuple_data) exit(-1);
	(*l_tups)->data = (lineitem_data*)tuple_data; // set tuple data in the lineitem struct

}

void fill(lineitem* l_tups, char** elems) {
	(*l_tups)->data[curr_tuple].l_shipdate = convert_date(elems[10]);
	(*l_tups)->data[curr_tuple].l_discount = (uint64_t)ceil(strtod(elems[6], &elems[6 + 1]) * 100); // double to int conversion
	(*l_tups)->data[curr_tuple].l_quantity = (uint64_t)(atoi(elems[3]));
	(*l_tups)->data[curr_tuple].l_extendedprice = (uint64_t)ceil(strtod(elems[5], &elems[5 + 1]));
}

#endif



#ifdef __COL
void allocate(lineitem** l_tups) {
	*l_tups = (lineitem*)malloc(sizeof(lineitem));
	

	// stores different string sizes
	// need to dynamically allocate the array
	size_t *string_szs = (size_t*)malloc(sizeof(size_t) * 3);
	size_t string_szs_set[3] = { 26, 11, 45 };
	memcpy(string_szs, string_szs_set, sizeof(size_t) * 3);

	td_setup(&(*l_tups)->td, 5, 3, 2, 3, 3, string_szs);
	td_allocate(&(*l_tups)->td);

	// big ints
	(*l_tups)->l_orderkey 		= BIGINT_MEMSET(*l_tups, 0);
	(*l_tups)->l_partkey 		= BIGINT_MEMSET(*l_tups, 1);
	(*l_tups)->l_suppkey 		= BIGINT_MEMSET(*l_tups, 2);
	(*l_tups)->l_linenumber 	= BIGINT_MEMSET(*l_tups, 3);
	(*l_tups)->l_quantity 		= BIGINT_MEMSET(*l_tups, 4);
	// doubles
	(*l_tups)->l_extendedprice 	= DOUBLE_MEMSET(*l_tups, 0); 
	(*l_tups)->l_discount 		= DOUBLE_MEMSET(*l_tups, 1); 
	(*l_tups)->l_tax 			= DOUBLE_MEMSET(*l_tups, 2); 
	// chars
	(*l_tups)->l_returnflag 	= CHAR_MEMSET(*l_tups, 0);
	(*l_tups)->l_linestatus 	= CHAR_MEMSET(*l_tups, 1);
	// dates
	(*l_tups)->l_shipdate 		= DATE_MEMSET(*l_tups, 0);
	(*l_tups)->l_commitdate 	= DATE_MEMSET(*l_tups, 1);
	(*l_tups)->l_receiptdate 	= DATE_MEMSET(*l_tups, 2);
	// strings
	(*l_tups)->l_shipinstruct 	= STRING_MEMSET(*l_tups, 0);
	(*l_tups)->l_shipmode 		= STRING_MEMSET(*l_tups, 1);
	(*l_tups)->l_comment 		= STRING_MEMSET(*l_tups, 2);
}

void fill(lineitem* l_tups, char** elems, uint64_t curr_tuple) {
	l_tups->l_orderkey[curr_tuple] 		= BIGINT_SET(elems[0]); 
	l_tups->l_partkey[curr_tuple] 		= BIGINT_SET(elems[1]); 
	l_tups->l_suppkey[curr_tuple] 		= BIGINT_SET(elems[2]); 
	l_tups->l_linenumber[curr_tuple] 	= BIGINT_SET(elems[3]); 
	l_tups->l_quantity[curr_tuple] 		= BIGINT_SET(elems[4]); 
	l_tups->l_extendedprice[curr_tuple] = DOUBLE_SET(elems[5], 1); 
	l_tups->l_discount[curr_tuple] 		= DOUBLE_SET(elems[6], 100); 
	l_tups->l_tax[curr_tuple] 			= DOUBLE_SET(elems[7], 100); 
	l_tups->l_returnflag[curr_tuple] 	= CHAR_SET(elems[8]);
	l_tups->l_linestatus[curr_tuple] 	= CHAR_SET(elems[9]);
	l_tups->l_shipdate[curr_tuple] 		= DATE_SET(elems[10]);
	l_tups->l_commitdate[curr_tuple] 	= DATE_SET(elems[11]);
	l_tups->l_receiptdate[curr_tuple] 	= DATE_SET(elems[12]);
	// strings
	strncpy((char *)GET_STRING(l_tups, l_shipinstruct, 0, curr_tuple), 
			elems[13], GET_STRING_SIZE(l_tups, 0));
	strncpy((char *)GET_STRING(l_tups, l_shipmode, 1, curr_tuple),
			elems[14], GET_STRING_SIZE(l_tups, 1));
	strncpy((char *)GET_STRING(l_tups, l_comment,  2, curr_tuple),
			elems[15], GET_STRING_SIZE(l_tups, 2));
}

void table_free(lineitem** l_tups) {
	td_free(&(*l_tups)->td);
	free(*l_tups);
}

#endif


void print_data(lineitem* l_tups) {
	for (uint32_t i = 0; i < l_tups->elements; i++) {
#ifdef __ROW
		std::cout << l_tups->data[i].l_shipdate << ", " <<
			l_tups->data[i].l_discount << ", " <<
			l_tups->data[i].l_quantity << ", " <<
			l_tups->data[i].l_extendedprice << std::endl;
#endif

#ifdef __COL
		std::cout <<
			l_tups->l_orderkey[i] 		__PIPE
			l_tups->l_partkey[i]    	__PIPE
			l_tups->l_suppkey[i]  		__PIPE 
			l_tups->l_linenumber[i] 	__PIPE
			l_tups->l_quantity[i] 		__PIPE 
			l_tups->l_extendedprice[i] 	__PIPE
			l_tups->l_discount[i] 		__PIPE 
			l_tups->l_tax[i]			__PIPE
			l_tups->l_returnflag[i] 	__PIPE 
			l_tups->l_linestatus[i] 	__PIPE
			l_tups->l_shipdate[i]		__PIPE 
			l_tups->l_commitdate[i]		__PIPE
			l_tups->l_receiptdate[i]	__PIPE 
			// if string
			GET_STRING(l_tups, l_shipinstruct, 0, i) __PIPE
			GET_STRING(l_tups, l_shipmode, 	   1, i) __PIPE 
			GET_STRING(l_tups, l_comment,  	   2, i) << 
		std::endl;
#endif
	}
}