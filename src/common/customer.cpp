#include <iostream>
#include <string>

#include <unistd.h>
#include <string.h>
#include <math.h>

#include <common/customer.h>
#include <common/tables.h>
#include <common/tpch.h>


// local functions
void allocate(customer** c_tups);
void fill(customer* c_tups, char** elems, uint64_t curr_tuple);
void database_write_binary(customer **c_tups, std::string db_bin);
int  database_read_binary(customer **c_tups, std::string db_bin);
DEFINE_DATABASE_READ(customer, CUSTOMER_COLUMNS);
DEFINE_DATABASE_RETRIEVE(customer, "customer.tbl");

void allocate(customer** c_tups) {
	*c_tups = (customer*)malloc(sizeof(customer));

	STRING_SIZE_SET(string_szs, 5, CUSTOMER_DBSTRING_SZS);
	td_setup(&(*c_tups)->td, 2, 1, 0, 0, 5, string_szs);
	td_allocate(&(*c_tups)->td);

	// big ints
	(*c_tups)->c_custkey 		= BIGINT_MEMSET(*c_tups, 0);
	(*c_tups)->c_nationkey 		= BIGINT_MEMSET(*c_tups, 1);
	// doubles
	(*c_tups)->c_acctbal 	    = DOUBLE_MEMSET(*c_tups, 0); 
	// strings
	(*c_tups)->c_name 	        = STRING_MEMSET(*c_tups, 0);
	(*c_tups)->c_address 		= STRING_MEMSET(*c_tups, 1);
	(*c_tups)->c_phone 		    = STRING_MEMSET(*c_tups, 2);
	(*c_tups)->c_mktsegment 	= STRING_MEMSET(*c_tups, 3);
	(*c_tups)->c_comment 		= STRING_MEMSET(*c_tups, 4);

}

int database_read_binary(customer **c_tups, std::string db_bin) {
	if (!access(db_bin.c_str(), F_OK)) {
		std::cout << "Found a binary .tbl file: " << db_bin << std::endl;
		FILE* f_bin = fopen(db_bin.c_str(), "rb");
		fseek(f_bin, 0L, SEEK_END);
		uint64_t bytes_read = ftell(f_bin);
		fseek(f_bin, 0L, SEEK_SET);

		uint32_t tuples = bytes_read / CUSTOMER_TUPLE_SIZE;
		(*c_tups)->elements = tuples;
		td_reallocate(&(*c_tups)->td, tuples);

		fread((void*)(*c_tups)->c_custkey, 		sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*c_tups)->c_name, 		GET_STRING_SIZE((*c_tups), 0), tuples, f_bin);
		fread((void*)(*c_tups)->c_address, 		GET_STRING_SIZE((*c_tups), 1), tuples, f_bin);
        fread((void*)(*c_tups)->c_nationkey, 	sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*c_tups)->c_phone, 		GET_STRING_SIZE((*c_tups), 2), tuples, f_bin);
		fread((void*)(*c_tups)->c_acctbal, 	    sizeof(__DOUBLE), tuples, f_bin);
		fread((void*)(*c_tups)->c_mktsegment,	GET_STRING_SIZE((*c_tups), 3), tuples, f_bin);
        fread((void*)(*c_tups)->c_comment, 		GET_STRING_SIZE((*c_tups), 4), tuples, f_bin);

		fclose(f_bin);
		// return 1 on success
		return 1;
	}
	// return on failure
	return 0;
}


void database_write_binary(customer **c_tups, std::string db_bin) {
	uint64_t tuples = (*c_tups)->elements;
	FILE* f_write = fopen(db_bin.c_str(), "wb");

	fwrite((void*)(*c_tups)->c_custkey, 		sizeof(__BIGINT), tuples, f_write);
	fwrite((void*)(*c_tups)->c_name, 			GET_STRING_SIZE((*c_tups), 0), tuples, f_write);
	fwrite((void*)(*c_tups)->c_address, 		GET_STRING_SIZE((*c_tups), 1), tuples, f_write);
	fwrite((void*)(*c_tups)->c_nationkey, 		sizeof(__BIGINT), tuples, f_write);
	fwrite((void*)(*c_tups)->c_phone, 			GET_STRING_SIZE((*c_tups), 2), tuples, f_write);
	fwrite((void*)(*c_tups)->c_acctbal, 	    sizeof(__DOUBLE), tuples, f_write);
	fwrite((void*)(*c_tups)->c_mktsegment,		GET_STRING_SIZE((*c_tups), 3), tuples, f_write);
	fwrite((void*)(*c_tups)->c_comment, 		GET_STRING_SIZE((*c_tups), 4), tuples, f_write);

	fclose(f_write);
}

void fill(customer* c_tups, char** elems, uint64_t curr_tuple) {
	c_tups->c_custkey[curr_tuple]   = BIGINT_SET(elems[0]);
    strncpy((char *)GET_STRING(c_tups, c_name, 0, curr_tuple),    elems[1], GET_STRING_SIZE(c_tups, 0)); 
    strncpy((char *)GET_STRING(c_tups, c_address, 1, curr_tuple), elems[2], GET_STRING_SIZE(c_tups, 1)); 
	c_tups->c_nationkey[curr_tuple] = BIGINT_SET(elems[3]); 
    strncpy((char *)GET_STRING(c_tups, c_phone, 2, curr_tuple),   elems[4], GET_STRING_SIZE(c_tups, 2)); 
    c_tups->c_acctbal[curr_tuple]   = DOUBLE_SET(elems[5], 1); 
    strncpy((char *)GET_STRING(c_tups, c_mktsegment,  3, curr_tuple), elems[6], GET_STRING_SIZE(c_tups, 3));
	strncpy((char *)GET_STRING(c_tups, c_comment,  4, curr_tuple),    elems[7], GET_STRING_SIZE(c_tups, 4));
}

void table_free(customer** c_tups) {
	td_free(&(*c_tups)->td);
	free(*c_tups);
}

void print_data(customer* c_tups) {
	for (uint32_t i = 0; i < c_tups->elements; i++) {
		std::cout <<
			c_tups->c_custkey[i] 		                __PIPE 
			GET_STRING(c_tups, c_name,          0, i)   __PIPE
			GET_STRING(c_tups, c_address, 	    1, i)   __PIPE 
            c_tups->c_nationkey[i] 	                    __PIPE
            GET_STRING(c_tups, c_phone,         2, i)   __PIPE
            c_tups->c_acctbal[i] 	                    __PIPE
			GET_STRING(c_tups, c_mktsegment,    3, i)   __PIPE 
			GET_STRING(c_tups, c_comment,  	    4, i)   << 
		std::endl;
	}
}