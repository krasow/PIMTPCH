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

#include "tpch.h"
#include "tables.h"

#define CUSTOMER_COLUMNS    8

#ifdef __COL
#define CUSTOMER_TUPLE_SIZE 148

typedef struct customer {
    uint32_t  elements;
    table_desc td;

    // different columns
    __BIGINT*  	 c_custkey;
    __DBSTRING*  c_name;
    __DBSTRING*  c_address;
    __BIGINT*    c_nationkey;
    __DBSTRING*  c_phone;
    __DOUBLE*    c_acctbal;
    __DBSTRING*  c_mktsegment;
    __DBSTRING*  c_comment;
} customer;

#endif

void retrieve(customer** c_tups);
void print_data(customer* c_tups);
void table_free(customer** c_tups);

#endif