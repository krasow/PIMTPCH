#ifndef ORDERS_H 
#define ORDERS_H

#include "tpch.h"

#define ORDERS_COLUMNS 16
#define MAX_TUPLES       (1<<25)

#ifdef __ROW
#define TUPLE_SIZE      32
typedef struct orders_data {
    uint64_t  	 o_orderkey;
    uint64_t     o_orderkey;
    u_char       o_orderstatus;
    uint64_t     o_totalprice;
    uint32_t     o_orderdate;
    u_char       o_orderpriority[16];
    u_char       o_clerk[16];
    uint32_t     o_shippriority;
    u_char       o_comment[80];
}  orders_data;

typedef struct orders {
    // data is used for memory management purposes
    orders_data*  data;
    uint32_t      elements;
} orders;

#endif

#ifdef __COL
#define TUPLE_SIZE      28

typedef struct orders {
    // data is used for memory management purposes
    char*     data;
    uint32_t  elements;

    // different columns
    uint64_t*  	 o_orderkey;
    uint64_t*    o_orderkey;
    char*        o_orderstatus;
    uint64_t*    o_totalprice;
    uint32_t*    o_orderdate;
    u_char*      o_orderpriority[16];
    u_char*      o_clerk[16];
    uint32_t*    o_shippriority;
    u_char*      o_comment[80];
} orders;

#endif

void retrieve(orders** o_tups);
void print_data(orders* o_tups);

#endif