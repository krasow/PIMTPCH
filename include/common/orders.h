#ifndef ORDERS_H 
#define ORDERS_H
/* Table schmema for lineitem
https://github.com/dimitri/tpch-citus/blob/master/schema/tpch-schema.sql

CREATE TABLE orders
(
    o_orderkey       BIGINT not null,
    o_custkey        BIGINT not null,
    o_orderstatus    CHAR(1) not null,
    o_totalprice     DOUBLE PRECISION not null,
    o_orderdate      DATE not null,
    o_orderpriority  CHAR(15) not null,  
    o_clerk          CHAR(15) not null, 
    o_shippriority   INTEGER not null,
    o_comment        VARCHAR(79) not null
);
/--------------------------------------------------/

CONVERSIONS USED 

BIGINT      -> uint64_t
DATE        -> uint32_t in epoch format
CHAR(x)     -> uchar_t[x+1] | x > 1
CHAR(1)     -> uchar_t
VARCHAR(x)  -> uchar_t[x+1]
DOUBLE      -> uint64_t

*/



#include "tpch.h"
#include "tables.h"

#define ORDERS_COLUMNS      9

#ifdef __COL
#define ORDERS_TUPLE_SIZE   148

typedef struct orders {
    uint32_t  elements;
    table_desc td;

    // different columns
    __BIGINT*  	 o_orderkey;
    __BIGINT*    o_custkey;
    __CHAR*      o_orderstatus;
    __DOUBLE*    o_totalprice;
    __DATE*      o_orderdate;
    __DBSTRING*  o_orderpriority;
    __DBSTRING*  o_clerk;
    __BIGINT*    o_shippriority;
    __DBSTRING*  o_comment;
} orders;

#endif

void retrieve(orders** o_tups);
void print_data(orders* o_tups);
void table_free(orders** o_tups);

#endif