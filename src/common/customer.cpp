#include <iostream>
#include <string>

#include <unistd.h>
#include <string.h>
#include <math.h>

#include <common/customer.h>
#include <common/tables.h>
#include <common/tpch.h>

void allocate(customer** c_tups);
void fill(customer* c_tups, char** elems, uint64_t curr_tuple);
void database_read(customer **c_tups, std::string db);
void database_write_binary(customer **c_tups, std::string db_bin);
int  database_read_binary(customer **c_tups, std::string db_bin);


void retrieve(customer** c_tups) {
	std::string db = DATABASE;
	db += "/customer.tbl";
	#ifdef __ROW
	    std::string db_bin = db + ".row.bin";
    #else
        std::string db_bin = db + ".bin";
    #endif

	allocate(c_tups);

	// check if there has been a binary file already produced from .tbl
	// only passes if table was loaded to the system before
	if(database_read_binary(c_tups, db_bin)) {
		return;
	}

	// read DATABASE/customer.tbl defined in common/tpch.h
	database_read(c_tups, db);
	// write the table to a binary file to avoid conversions for reruns
	database_write_binary(c_tups, db_bin);
}


void allocate(customer** c_tups) {
	*c_tups = (customer*)malloc(sizeof(customer));

	// stores different string sizes
	// need to dynamically allocate the array
	size_t *string_szs = (size_t*)malloc(sizeof(size_t) * 5);
	size_t string_szs_set[5] = { 25, 40, 15, 10, 117 };
	memcpy(string_szs, string_szs_set, sizeof(size_t) * 5);

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


void database_read(customer **c_tups, std::string db) {
	std::cout << db << std::endl;

	if (access(db.c_str(), F_OK)) {
		std::cerr << "Error: no .tbl database folder found." << std::endl;
		exit(12);
	}

	// hold an entire row from fgets
	char buffer[ROW_BUFFER];
	// hold an entire row separated by pipe delimiter & heap allocation
	char **elems = (char**)malloc(CUSTOMER_COLUMNS * sizeof(char*));
	
	for (uint16_t i = 0; i < CUSTOMER_COLUMNS; i++) {
		elems[i] = (char*)malloc(COLUMN_BUFFER);
	}

	uint64_t curr_tuple = 0;
	FILE* f = fopen(db.c_str(), "rb");
	// goes thru all the tuples
	while (fgets(buffer, ROW_BUFFER, f) && curr_tuple < MAX_TUPLES) {
		uint16_t curr_elem = 0;
		int bytes = 0;
		// store one column from the buffer ("STORE THIS"|"NOT THIS"|)
		char tmp[COLUMN_BUFFER];

		// used to increment the buffer
		char* loc_buffer = buffer;
		// for each element in each tuple
		while (curr_elem < CUSTOMER_COLUMNS && 
			   sscanf(loc_buffer, "%[^|]|%n", tmp, &bytes) == 1) 
		{
			strncpy(elems[curr_elem], tmp, bytes);
			elems[curr_elem][bytes+1] = '\0'; // add NULL terminating character
			std::cout << tmp << std::endl;
			curr_elem++;
			loc_buffer += bytes;
		}

		// fill elements into c_tups data struct
		fill(*c_tups, elems, curr_tuple);
		curr_tuple++;
	}

	for (uint16_t i = 0; i < CUSTOMER_COLUMNS; i++) {
		free(elems[i]);
	}
	free(elems);
	fclose(f);


	// set amount of tuples read
	(*c_tups)->elements = curr_tuple;
	// reallocate based on elements read
	// will be lt or eq NOT gt
	td_reallocate(&(*c_tups)->td, (*c_tups)->elements);
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
    fwrite((void*)(*c_tups)->c_name, 		    GET_STRING_SIZE((*c_tups), 0), tuples, f_write);
    fwrite((void*)(*c_tups)->c_address, 		GET_STRING_SIZE((*c_tups), 1), tuples, f_write);
    fwrite((void*)(*c_tups)->c_nationkey, 	    sizeof(__BIGINT), tuples, f_write);
    fwrite((void*)(*c_tups)->c_phone, 		    GET_STRING_SIZE((*c_tups), 2), tuples, f_write);
    fwrite((void*)(*c_tups)->c_acctbal, 	    sizeof(__DOUBLE), tuples, f_write);
    fwrite((void*)(*c_tups)->c_mktsegment,	    GET_STRING_SIZE((*c_tups), 3), tuples, f_write);
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