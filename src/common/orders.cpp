#include <iostream>
#include <string>

#include <unistd.h>
#include <string.h>
#include <math.h>

#include <common/orders.h>
#include <common/tables.h>
#include <common/tpch.h>

void allocate(orders** o_tups);
void fill(orders* o_tups, char** elems, uint64_t curr_tuple);
void database_read(orders **o_tups, std::string db);
void database_write_binary(orders **o_tups, std::string db_bin);
int  database_read_binary(orders **o_tups, std::string db_bin);


void retrieve(orders** o_tups) {
	std::string db = DATABASE;
	db += "/orders.tbl";
	#ifdef __ROW
	    std::string db_bin = db + ".row.bin";
    #else
        std::string db_bin = db + ".bin";
    #endif

	allocate(o_tups);

	// check if there has been a binary file already produced from .tbl
	// only passes if table was loaded to the system before
	if(database_read_binary(o_tups, db_bin)) {
		return;
	}

	// read DATABASE/orders.tbl defined in common/tpch.h
	database_read(o_tups, db);
	// write the table to a binary file to avoid conversions for reruns
	database_write_binary(o_tups, db_bin);
}


void allocate(orders** o_tups) {
	*o_tups = (orders*)malloc(sizeof(orders));

	// stores different string sizes
	// need to dynamically allocate the array
	size_t *string_szs = (size_t*)malloc(sizeof(size_t) * 3);
	size_t string_szs_set[3] = { 16, 16, 79 };
	memcpy(string_szs, string_szs_set, sizeof(size_t) * 3);

	td_setup(&(*o_tups)->td, 3, 1, 1, 1, 3, string_szs);
	td_allocate(&(*o_tups)->td);

	// big ints
	(*o_tups)->o_orderkey 		= BIGINT_MEMSET(*o_tups, 0);
	(*o_tups)->o_custkey 		= BIGINT_MEMSET(*o_tups, 1);
	(*o_tups)->o_shippriority 	= BIGINT_MEMSET(*o_tups, 2);
	// doubles
	(*o_tups)->o_totalprice 	= DOUBLE_MEMSET(*o_tups, 0); 
	// chars
	(*o_tups)->o_orderstatus 	= CHAR_MEMSET(*o_tups, 0);
	// dates
	(*o_tups)->o_orderdate 		= DATE_MEMSET(*o_tups, 0);
	// strings
	(*o_tups)->o_orderpriority 	= STRING_MEMSET(*o_tups, 0);
	(*o_tups)->o_clerk 		    = STRING_MEMSET(*o_tups, 1);
	(*o_tups)->o_comment 		= STRING_MEMSET(*o_tups, 2);
}


void database_read(orders **o_tups, std::string db) {
	std::cout << db << std::endl;

	if (access(db.c_str(), F_OK)) {
		std::cerr << "Error: no .tbl database folder found." << std::endl;
		exit(12);
	}

	// hold an entire row from fgets
	char buffer[ROW_BUFFER];
	// hold an entire row separated by pipe delimiter & heap allocation
	char **elems = (char**)malloc(ORDERS_COLUMNS * sizeof(char*));
	
	for (uint16_t i = 0; i < ORDERS_COLUMNS; i++) {
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
		while (curr_elem < ORDERS_COLUMNS && 
			   sscanf(loc_buffer, "%[^|]|%n", tmp, &bytes) == 1) 
		{
			strncpy(elems[curr_elem], tmp, bytes);
			elems[curr_elem][bytes+1] = '\0'; // add NULL terminating character
			// std::cout << tmp << std::endl;
			curr_elem++;
			loc_buffer += bytes;
		}

		// fill elements into o_tups data struct
		fill(*o_tups, elems, curr_tuple);
		curr_tuple++;
	}

	for (uint16_t i = 0; i < ORDERS_COLUMNS; i++) {
		free(elems[i]);
	}
	free(elems);
	fclose(f);


	// set amount of tuples read
	(*o_tups)->elements = curr_tuple;
	// reallocate based on elements read
	// will be lt or eq NOT gt
	td_reallocate(&(*o_tups)->td, (*o_tups)->elements);
}




int database_read_binary(orders **o_tups, std::string db_bin) {
	if (!access(db_bin.c_str(), F_OK)) {
		std::cout << "Found a binary .tbl file: " << db_bin << std::endl;
		FILE* f_bin = fopen(db_bin.c_str(), "rb");
		fseek(f_bin, 0L, SEEK_END);
		uint64_t bytes_read = ftell(f_bin);
		fseek(f_bin, 0L, SEEK_SET);

		uint32_t tuples = bytes_read / ORDERS_TUPLE_SIZE;
		(*o_tups)->elements = tuples;
		td_reallocate(&(*o_tups)->td, tuples);

		fread((void*)(*o_tups)->o_orderkey, 		sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*o_tups)->o_custkey, 			sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*o_tups)->o_orderstatus, 		sizeof(__CHAR),  tuples, f_bin);
        fread((void*)(*o_tups)->o_totalprice, 	    sizeof(__DOUBLE), tuples, f_bin);
		fread((void*)(*o_tups)->o_orderdate, 		sizeof(__DATE),	 tuples, f_bin);
		fread((void*)(*o_tups)->o_orderpriority, 	GET_STRING_SIZE((*o_tups), 0), tuples, f_bin);
		fread((void*)(*o_tups)->o_clerk,			GET_STRING_SIZE((*o_tups), 1), tuples, f_bin);
        fread((void*)(*o_tups)->o_shippriority, 	sizeof(__BIGINT), tuples, f_bin);
		fread((void*)(*o_tups)->o_comment, 			GET_STRING_SIZE((*o_tups), 2), tuples, f_bin);

		fclose(f_bin);
		// return 1 on success
		return 1;
	}
	// return on failure
	return 0;
}


void database_write_binary(orders **o_tups, std::string db_bin) {
	uint64_t tuples = (*o_tups)->elements;

	FILE* f_write = fopen(db_bin.c_str(), "wb");


    fwrite((void*)(*o_tups)->o_orderkey, 		sizeof(__BIGINT), tuples, f_write);
    fwrite((void*)(*o_tups)->o_custkey, 		sizeof(__BIGINT), tuples, f_write);
    fwrite((void*)(*o_tups)->o_orderstatus, 	sizeof(__CHAR),  tuples, f_write);
    fwrite((void*)(*o_tups)->o_totalprice, 	    sizeof(__DOUBLE), tuples, f_write);
    fwrite((void*)(*o_tups)->o_orderdate, 		sizeof(__DATE),	 tuples, f_write);
    fwrite((void*)(*o_tups)->o_orderpriority, 	GET_STRING_SIZE((*o_tups), 0), tuples, f_write);
    fwrite((void*)(*o_tups)->o_clerk,			GET_STRING_SIZE((*o_tups), 1), tuples, f_write);
    fwrite((void*)(*o_tups)->o_shippriority, 	sizeof(__BIGINT), tuples, f_write);
    fwrite((void*)(*o_tups)->o_comment, 		GET_STRING_SIZE((*o_tups), 2), tuples, f_write);

	fclose(f_write);
}



void fill(orders* o_tups, char** elems, uint64_t curr_tuple) {
	o_tups->o_orderkey[curr_tuple] 		= BIGINT_SET(elems[0]); 
	o_tups->o_custkey[curr_tuple] 		= BIGINT_SET(elems[1]); 
	o_tups->o_orderstatus[curr_tuple]   = CHAR_SET(elems[2]);
    o_tups->o_totalprice[curr_tuple]    = DOUBLE_SET(elems[3], 1); 
	o_tups->o_orderdate[curr_tuple] 	= DATE_SET(elems[4]);
	strncpy((char *)GET_STRING(o_tups, o_orderpriority, 0, curr_tuple), 
			elems[5], GET_STRING_SIZE(o_tups, 0));
	strncpy((char *)GET_STRING(o_tups, o_clerk, 1, curr_tuple),
			elems[6], GET_STRING_SIZE(o_tups, 1));
    o_tups->o_shippriority[curr_tuple] 	= BIGINT_SET(elems[7]); 
	strncpy((char *)GET_STRING(o_tups, o_comment,  2, curr_tuple),
			elems[8], GET_STRING_SIZE(o_tups, 2));
}

void table_free(orders** o_tups) {
	td_free(&(*o_tups)->td);
	free(*o_tups);
}

void print_data(orders* o_tups) {
	for (uint32_t i = 0; i < o_tups->elements; i++) {
		std::cout <<
			o_tups->o_orderkey[i] 		__PIPE
			o_tups->o_custkey[i]    	__PIPE
			o_tups->o_orderstatus[i]  	__PIPE 
			o_tups->o_totalprice[i] 	__PIPE
			o_tups->o_orderdate[i] 		__PIPE 
			GET_STRING(o_tups, o_orderpriority, 0, i) __PIPE
			GET_STRING(o_tups, o_clerk, 	    1, i) __PIPE 
            o_tups->o_shippriority[i] 	__PIPE
			GET_STRING(o_tups, o_comment,  	    2, i) << 
		std::endl;
	}
}