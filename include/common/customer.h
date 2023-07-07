#ifndef CUSTOMER_H 
#define CUSTOMER_H

#include "tpch.h"

#define CUSTOMER_COLUMNS 16
#define MAX_TUPLES       (1<<25)

#ifdef __ROW
//size is 32 bytes alligned (one tuple)
#define TUPLE_SIZE      32
typedef struct customer_data {
    uint32_t     l_shipdate;
    uint64_t  	 l_discount;

    uint64_t     l_quantity;
    uint64_t     l_extendedprice;
} __attribute__((aligned(32))) customer_data;

typedef struct customer {
    // data is used for memory management purposes
    customer_data*  data;
    uint32_t        elements;
} customer;

#endif

#ifdef __COL
//one tuple is 28 bytes
#define TUPLE_SIZE      28

typedef struct customer {
    // data is used for memory management purposes
    char*     data;
    uint32_t  elements;

    // different columns
    uint32_t* l_shipdate;
    uint64_t* l_discount;
    uint64_t* l_quantity;
    uint64_t* l_extendedprice;
} customer;

#endif

void retrieve(customer** c_tups);
void print_data(customer* c_tups);

#endif