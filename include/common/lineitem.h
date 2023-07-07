#ifndef LINEITEM_H 
#define LINEITEM_H
/* Table schmema for lineitem
https://github.com/dimitri/tpch-citus/blob/master/schema/tpch-schema.sql

CREATE TABLE lineitem
(
    l_orderkey    BIGINT not null,
    l_partkey     BIGINT not null,
    l_suppkey     BIGINT not null,
    l_linenumber  BIGINT not null,
    l_quantity    DOUBLE PRECISION not null,
    l_extendedprice  DOUBLE PRECISION not null,
    l_discount    DOUBLE PRECISION not null,
    l_tax         DOUBLE PRECISION not null,
    l_returnflag  CHAR(1) not null,
    l_linestatus  CHAR(1) not null,
    l_shipdate    DATE not null,
    l_commitdate  DATE not null,
    l_receiptdate DATE not null,
    l_shipinstruct CHAR(25) not null,
    l_shipmode     CHAR(10) not null,
    l_comment      VARCHAR(44) not null
);
/--------------------------------------------------/

CONVERSIONS USED 

BIGINT      -> uint64_t
DATE        -> uint32_t in epoch format
CHAR(x)     -> uchar_t[x] | x > 1
CHAR(1)     -> uchar_t
VARCHAR(x)  -> uchar_t[x]
DOUBLE      -> uint64_t

*/


#include "tpch.h"
#include "tables.h"

#define LINEITEM_COLUMNS 16

#ifdef __ROW
//size is 32 bytes alligned (one tuple)
#define TUPLE_SIZE      32
typedef struct lineitem_data {
    uint32_t     l_shipdate;
    uint64_t  	 l_discount;
    uint64_t     l_quantity;
    uint64_t     l_extendedprice;
} __attribute__((aligned(32))) lineitem_data;

typedef struct lineitem {
    // data is used for memory management purposes
    lineitem_data*  data;
    uint32_t        elements;
} lineitem;

#endif

#ifdef __COL
//one tuple is 160 bytes
#define TUPLE_SIZE      160

typedef struct lineitem {
    uint32_t  elements;
    table_desc td;

    // different columns
    uint64_t* l_orderkey;
    uint64_t* l_partkey;
    uint64_t* l_suppkey;
    uint64_t* l_linenumber;
    uint64_t* l_quantity;
    uint64_t* l_extendedprice;
    uint64_t* l_discount;
    uint64_t* l_tax;            // 64 bytes
    uchar_t*  l_returnflag;
    uchar_t*  l_linestatus;     // 66 bytes
    uint32_t* l_shipdate;
    uint32_t* l_commitdate;
    uint32_t* l_receiptdate;    // 78 bytes
    uchar_t** l_shipinstruct;
    uchar_t** l_shipmode;
    uchar_t** l_comment;        // 160 bytes
} lineitem;

#endif

void retrieve(lineitem** l_tups);
void print_data(lineitem* l_tups);
void table_free(lineitem** l_tups);

#endif