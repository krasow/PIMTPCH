#ifndef TPCH_H 
#define TPCH_H

#include <stdlib.h>
#include <stdint.h>

// #define DEBUG
// #define PRINT

#define __COL
#define DATABASE		"/home/david/PIM_TPCH/database1"

#define MAX_TUPLES       (1<<25)

#define SEED			72
#define PAGE_SIZE       4096


#define divceil(n, m)   (((n)-1) / (m) + 1)
#define roundup(n, m)   ((n / m) * m + m)
#define __PIPE          << " | " <<

// if __ROW or __COL is not defined, __COL will be default
#ifndef  __ROW
#define __COL 
#endif

typedef unsigned char uchar_t;
typedef uint64_t addr_t;

#endif