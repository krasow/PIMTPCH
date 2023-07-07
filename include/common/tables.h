#ifndef TABLES_H 
#define TABLES_H

#include "tpch.h"

typedef struct td_elem {
    addr_t*  items;
    uint16_t cnt;
    size_t   size;  // used if fixed size
    size_t*  sizes; // used if non-fixed size
} td_elem;

typedef struct table_desc {
    td_elem doubles;
    td_elem bigInts;
    td_elem dates;
    td_elem chars;
    td_elem strings;
} table_desc;


void td_setup(table_desc* td,
    size_t bigInts,
    size_t doubles,
    size_t chars,
    size_t dates,
    size_t strings,
    size_t* string_szs);
void td_allocate(table_desc* td);
void td_reallocate(table_desc* td, uint64_t tuple_cnt);
void td_free(table_desc* td);


uint32_t convert_date(char* date);

#endif