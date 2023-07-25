#ifndef Q6_H 
#define Q6_H

#include "tpch.h"
#include "common/lineitem.h"
#include "common/tables.h"

// Q6 query selectivity 
#define Q6_DATE1        757382400  // "1994-01-01"
#define Q6_DATE2        788918400  // "1995-01-01"
#define Q6_DISCOUNT1    (6 - 1)    // 0.06 - 0.01
#define Q6_DISCOUNT2    (6 + 1)    // 0.06 + 0.01
#define Q6_QUANTITY     24


#define Q6_TXF_SIZE     32  // transfer size of l_tup for Q6 (4 is padding)
#define Q6_ACT_SIZE     28  // actual size of l_tup for Q6



// if using CPU
#if (!DPU)
uint64_t q6(const lineitem* l_tups);
uint64_t q6_naive(const lineitem* l_tups);
void q6_selectivity_print(const lineitem* l_tups);
#endif

// using upmem PIM modules
#if DPU 
#define Q6_DPU_BINARY "./build/upmem/q6_dpu"

typedef struct {
    uint32_t l_tups_cnt;
    uint32_t transfer_size;
} q6_dpu_arguments_t;

#endif 


#endif