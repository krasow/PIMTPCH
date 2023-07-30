#ifndef TPCH_H 
#define TPCH_H

#include <stdlib.h>
#include <stdint.h>

// #define DEBUG
// #define PRINT

#define __COL
// #define __SCALE10

#define __BIGINT    uint64_t
#define __DOUBLE    uint64_t
#define __CHAR      uchar_t
#define __DATE      uint32_t
#define __DBSTRING  uchar_t

#define GET_STRING_SIZE(table, string_id)       table->td->strings.sizes[string_id]
#define GET_STRING(table, column, string_id, i) &table->column[GET_STRING_SIZE(table, string_id) * i]

#ifdef __SCALE10
#define DATABASE		"/home/david/PIM_TPCH/sf10"
#define MAX_TUPLES       (1<<26)
#else 
#define DATABASE		"/home/david/PIM_TPCH/sf1"
#define MAX_TUPLES       (1<<23)
#endif

#define SEED			72
#define PAGE_SIZE       4096


#define divceil(n, m)   (((n)-1) / (m) + 1)
#define roundup(n, m)   ((n / m) * m + m)
#define __PIPE          << "|" <<

// if __ROW or __COL is not defined, __COL will be default
#ifndef  __ROW
#define __COL 
#endif

typedef unsigned char uchar_t;
typedef uint64_t addr_t;

uint16_t find_log2(uint16_t input);

#endif
