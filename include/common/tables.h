#ifndef TABLES_H 
#define TABLES_H

#include "tpch.h"


#define COLUMN_BUFFER 128
#define ROW_BUFFER 1000

#define BIGINT_MEMSET(table, id)    (__BIGINT*) (table)->td.bigInts.items[id]
#define DOUBLE_MEMSET(table, id)    (__DOUBLE*) (table)->td.doubles.items[id]
#define CHAR_MEMSET(table, id)      (__CHAR*)   (table)->td.chars.items[id]
#define DATE_MEMSET(table, id)      (__DATE*)   (table)->td.dates.items[id]
#define STRING_MEMSET(table, id)    (__DBSTRING*)(table)->td.strings.items[id]

#define BIGINT_SET(val)             (__BIGINT)(atoi(val))
#define DOUBLE_SET(val, scale)      (__DOUBLE)ceil(strtod(val, &val + COLUMN_BUFFER) * scale)
#define CHAR_SET(val)               val[0]
#define DATE_SET(val)               convert_date(val)

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