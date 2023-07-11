#ifndef Q1_H 
#define Q1_H

#include "tpch.h"
#include "lineitem.h"
#include "hmap.h"
#include "tables.h"


#define Q1_DATE1    912492000 //1998-12-01



typedef struct values {
    __BIGINT* ints;
    __DOUBLE* doubles;

    uint16_t int_cnt;
    uint16_t double_cnt;  
} values;

#define val_malloc(val, num_ints, num_doubles) \
    val = (values*) malloc(sizeof(values)); \
    val->ints = (__BIGINT*) malloc(sizeof(__BIGINT) * num_ints); \
    val->doubles = (__DOUBLE*) malloc(sizeof(__DOUBLE) * num_doubles); \
    val->int_cnt = num_ints; \
    val->double_cnt = num_doubles; \




#define val_free(val) \
    free(val->ints); \
    free(val->doubles); \
    free(val); \



#define val_i64(val, offset)  val->ints[offset]
#define val_f64(val, offset)  val->doubles[offset]


#endif