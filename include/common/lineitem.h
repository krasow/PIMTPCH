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
CHAR(x)     -> uchar_t[x+1] | x > 1
CHAR(1)     -> uchar_t
VARCHAR(x)  -> uchar_t[x+1]
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
#define LINEITEM_TUPLE_SIZE     160

typedef struct lineitem {
    uint32_t  elements;
    table_desc td;

    // different columns
    __BIGINT*   l_orderkey;
    __BIGINT*   l_partkey;
    __BIGINT*   l_suppkey;
    __BIGINT*   l_linenumber;
    __BIGINT*   l_quantity;
    __DOUBLE*   l_extendedprice;
    __DOUBLE*   l_discount;
    __DOUBLE*   l_tax;            // 64 bytes
    __CHAR*     l_returnflag;
    __CHAR*     l_linestatus;     // 66 bytes
    __DATE*     l_shipdate;
    __DATE*     l_commitdate;
    __DATE*     l_receiptdate;    // 78 bytes
    __DBSTRING* l_shipinstruct;
    __DBSTRING* l_shipmode;
    __DBSTRING* l_comment;        // 160 bytes
} lineitem;

#endif

void retrieve(lineitem** l_tups);
void print_data(lineitem* l_tups);
void table_free(lineitem** l_tups);

#endif