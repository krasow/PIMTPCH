#ifndef Q6_TEST_H 
#define Q6_TEST_H

#include <stdlib.h>
#include <stdint.h>

// #define DEBUG
// #define PRINT

#define __ROW

#define SEED			72
#define PAGE_SIZE       4096
#define NUM_TUPLES      (1<<24)


// if __ROW or __COL is not defined, __COL will be default
#ifdef  __ROW
    #define DATABASE		"lineitems_row.dat"
#else 
    #define __COL 
    #define DATABASE		"lineitems.dat"
#endif


// Q6 query selectivity 
#define Q6_DATE1        757404000  // "1994-01-01"
#define Q6_DATE2        788940000  // "1995-01-01"
#define Q6_DISCOUNT1    (6 - 1)   // 0.06 - 0.01
#define Q6_DISCOUNT2    (6 + 1)   // 0.06 + 0.01
#define Q6_QUANTITY     24


#define divceil(n, m)   (((n)-1) / (m) + 1)
#define roundup(n, m)   ((n / m) * m + m)


#ifdef __ROW
//size is 32 bytes 
typedef struct data {
    uint32_t     l_shipdate;
    uint64_t  	 l_discount;
    uint64_t     l_quantity;
    uint64_t     l_extendedprice;
    char         junk[4];
}  data;
#endif

#ifdef __COL
typedef struct data {
    uint32_t     l_shipdate[NUM_TUPLES];
    uint64_t  	 l_discount[NUM_TUPLES];
    uint64_t     l_quantity[NUM_TUPLES];
    uint64_t     l_extendedprice[NUM_TUPLES];
} __attribute__((aligned(512))) data;
#endif

data* retrieve();
void print_data(data* lineitem);

#endif