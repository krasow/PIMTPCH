#ifndef TABLES_H 
#define TABLES_H

#include "tpch.h"

#define COLUMN_BUFFER 128
#define ROW_BUFFER 1000

#define GET_STRING_SIZE(table, string_id)       table->td.strings.sizes[string_id]
#define GET_STRING(table, column, string_id, i) &table->column[GET_STRING_SIZE(table, string_id) * i]

#define BIGINT_MEMSET(table, id)    (uint64_t*)(table)->td.bigInts.items[id]
#define DOUBLE_MEMSET(table, id)    (uint64_t*)(table)->td.doubles.items[id]
#define CHAR_MEMSET(table, id)      (uchar_t*)(table)->td.chars.items[id]
#define DATE_MEMSET(table, id)      (uint32_t*)(table)->td.dates.items[id]
#define STRING_MEMSET(table, id)    (uchar_t*)(table)->td.strings.items[id]

#define BIGINT_SET(val)             (uint64_t)(atoi(val))
#define DOUBLE_SET(val, scale)      (uint64_t)ceil(strtod(val, &val + COLUMN_BUFFER) * scale)
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