#include <iostream>
#include <string>

#include <unistd.h>
#include <string.h>
#include <math.h>

#include <common/lineitem.h>

//local functions
void allocate(lineitem** l_tups);
void fill(lineitem* l_tups, char** elems, uint64_t curr_tuple);
void database_write_binary(lineitem **l_tups, std::string db_bin);
int  database_read_binary(lineitem **l_tups, std::string db_bin);
DEFINE_DATABASE_READ(lineitem, LINEITEM_COLUMNS);
DEFINE_DATABASE_RETRIEVE(lineitem, "lineitem.tbl");
DEFINE_DATABASE_FREE(lineitem);

void allocate(lineitem** l_tups) {
	*l_tups = (lineitem*)malloc(sizeof(lineitem));
	table_desc* td = (table_desc*)malloc(sizeof(table_desc));
	(*l_tups)->td = td;

	STRING_SIZE_SET(string_szs, 3, LINEITEM_DBSTRING_SZS);
	td_setup((*l_tups)->td, 5, 3, 2, 3, 3, string_szs);
	td_allocate((*l_tups)->td);
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

int database_read_binary(lineitem **l_tups, std::string db_bin) {
	if (!access(db_bin.c_str(), F_OK)) {
		std::cout << "Found a binary .tbl file: " << db_bin << std::endl;
		FILE* f_bin = fopen(db_bin.c_str(), "rb");
		fseek(f_bin, 0L, SEEK_END);
		uint64_t bytes_read = ftell(f_bin);
		fseek(f_bin, 0L, SEEK_SET);

		uint32_t tuples = bytes_read / LINEITEM_TUPLE_SIZE;
		(*l_tups)->elements = tuples;
		td_reallocate((*l_tups)->td, tuples);

		fread((void*)(*l_tups)->l_orderkey, 		sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*l_tups)->l_partkey, 			sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*l_tups)->l_suppkey, 			sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*l_tups)->l_linenumber, 		sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*l_tups)->l_quantity, 		sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*l_tups)->l_extendedprice, 	sizeof(__DOUBLE), tuples, f_bin);
		fread((void*)(*l_tups)->l_discount, 		sizeof(__DOUBLE), tuples, f_bin);
		fread((void*)(*l_tups)->l_tax, 				sizeof(__DOUBLE), tuples, f_bin);
		fread((void*)(*l_tups)->l_returnflag, 		sizeof(__CHAR),  tuples, f_bin);
		fread((void*)(*l_tups)->l_linestatus, 		sizeof(__CHAR),  tuples, f_bin);
		fread((void*)(*l_tups)->l_shipdate, 		sizeof(__DATE),	 tuples, f_bin);
		fread((void*)(*l_tups)->l_commitdate, 		sizeof(__DATE),  tuples, f_bin);
		fread((void*)(*l_tups)->l_receiptdate, 		sizeof(__DATE),  tuples, f_bin);
		fread((void*)(*l_tups)->l_shipinstruct, 	GET_STRING_SIZE((*l_tups), 0), tuples, f_bin);
		fread((void*)(*l_tups)->l_shipmode,			GET_STRING_SIZE((*l_tups), 1), tuples, f_bin);
		fread((void*)(*l_tups)->l_comment, 			GET_STRING_SIZE((*l_tups), 2), tuples, f_bin);

		fclose(f_bin);
		// return 1 on success
		return 1;
	}
	// return on failure
	return 0;
}


void database_write_binary(lineitem **l_tups, std::string db_bin) {
	uint64_t tuples = (*l_tups)->elements;
	FILE* f_write = fopen(db_bin.c_str(), "wb");
	
	fwrite((void*)(*l_tups)->l_orderkey, 		sizeof(__BIGINT), tuples, f_write);
	fwrite((void*)(*l_tups)->l_partkey, 		sizeof(__BIGINT), tuples, f_write);
	fwrite((void*)(*l_tups)->l_suppkey, 		sizeof(__BIGINT), tuples, f_write);
	fwrite((void*)(*l_tups)->l_linenumber,		sizeof(__BIGINT), tuples, f_write);
	fwrite((void*)(*l_tups)->l_quantity, 		sizeof(__BIGINT), tuples, f_write);
	fwrite((void*)(*l_tups)->l_extendedprice, 	sizeof(__DOUBLE), tuples, f_write);
	fwrite((void*)(*l_tups)->l_discount, 		sizeof(__DOUBLE), tuples, f_write);
	fwrite((void*)(*l_tups)->l_tax, 			sizeof(__DOUBLE), tuples, f_write);
	fwrite((void*)(*l_tups)->l_returnflag, 		sizeof(__CHAR),  tuples, f_write);
	fwrite((void*)(*l_tups)->l_linestatus, 		sizeof(__CHAR),  tuples, f_write);
	fwrite((void*)(*l_tups)->l_shipdate, 		sizeof(__DATE), tuples, f_write);
	fwrite((void*)(*l_tups)->l_commitdate, 		sizeof(__DATE), tuples, f_write);
	fwrite((void*)(*l_tups)->l_receiptdate, 	sizeof(__DATE), tuples, f_write);
	fwrite((void*)(*l_tups)->l_shipinstruct, 	GET_STRING_SIZE((*l_tups), 0), tuples, f_write);
	fwrite((void*)(*l_tups)->l_shipmode, 		GET_STRING_SIZE((*l_tups), 1), tuples, f_write);
	fwrite((void*)(*l_tups)->l_comment, 		GET_STRING_SIZE((*l_tups), 2), tuples, f_write);

	fclose(f_write);
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
	// strings copying 
	strncpy((char *)GET_STRING(l_tups, l_shipinstruct, 0, curr_tuple), 
			elems[13], GET_STRING_SIZE(l_tups, 0));
	strncpy((char *)GET_STRING(l_tups, l_shipmode, 1, curr_tuple),
			elems[14], GET_STRING_SIZE(l_tups, 1));
	strncpy((char *)GET_STRING(l_tups, l_comment,  2, curr_tuple),
			elems[15], GET_STRING_SIZE(l_tups, 2));
}

void print_data(lineitem* l_tups) {
	for (uint32_t i = 0; i < l_tups->elements; i++) {
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
	}
}