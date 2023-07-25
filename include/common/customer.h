#ifndef CUSTOMER_H 
#define CUSTOMER_H
/* Table schmema for lineitem
https://github.com/dimitri/tpch-citus/blob/master/schema/tpch-schema.sql

CREATE TABLE customer
(
    c_custkey     BIGINT not null,
    c_name        VARCHAR(25) not null,
    c_address     VARCHAR(40) not null,
    c_nationkey   INTEGER not null,
    c_phone       CHAR(15) not null,
    c_acctbal     DOUBLE PRECISION   not null,
    c_mktsegment  CHAR(10) not null,
    c_comment     VARCHAR(117) not null
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

#include "tables.h"

#define CUSTOMER_COLUMNS    8

#ifdef __COL
#define CUSTOMER_TUPLE_SIZE     236
#define CUSTOMER_DBSTRING_SZS   {26, 41, 16, 11, 118}

typedef struct customer {
    uint32_t  elements;
    table_desc* td;

    // different columns
    __BIGINT*  	 c_custkey;         // 8
    __DBSTRING*  c_name;            // 34
    __DBSTRING*  c_address;         // 75
    __BIGINT*    c_nationkey;       // 83
    __DBSTRING*  c_phone;           // 99
    __DOUBLE*    c_acctbal;         // 107
    __DBSTRING*  c_mktsegment;      // 118
    __DBSTRING*  c_comment;         // 236 bytes
} customer;

#endif

void retrieve(customer** c_tups);
void print_data(customer* c_tups);
void table_free(customer** c_tups);

#endif