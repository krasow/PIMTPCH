#ifndef Q1_H 
#define Q1_H

#include "tpch.h"
#include "common/lineitem.h"
#include "common/hmap.h"
#include "common/tables.h"


#define Q1_DATE1    912470400 //1998-12-01


// if using CPU
#if (!DPU) 
void q1(struct tpch_hashtable **out, const lineitem * l_tups);
void q1_naive(struct tpch_hashtable **out, const lineitem * l_tups);
#endif

// using upmem PIM modules
#if DPU 
#define Q1_DPU_BINARY "./build/upmem/q1_dpu"
#endif
    
#endif