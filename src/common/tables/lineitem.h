#ifndef LINEITEM_H 
#define LINEITEM_H

#include "../tpch.h"

#ifdef __ROW
//size is 32 bytes alligned (one tuple)
#define TUPLE_SIZE      32
typedef struct lineitem {
    uint32_t     l_shipdate;
    uint64_t  	 l_discount;
    uint64_t     l_quantity;
    uint64_t     l_extendedprice;
} __attribute__((aligned(32))) lineitem;

#endif

#ifdef __COL
//one tuple is 28 bytes
#define TUPLE_SIZE      28

typedef struct lineitem {
    // data is used for memory management purposes
    char* data;

    // different columns
    uint32_t* l_shipdate;
    uint64_t* l_discount;
    uint64_t* l_quantity;
    uint64_t* l_extendedprice;
} lineitem;

#endif

void retrieve(lineitem** l_tups);
void print_data(lineitem* l_tups);

#endif