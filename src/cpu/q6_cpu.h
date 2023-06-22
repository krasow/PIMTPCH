#ifndef Q6_CPU_H 
#define Q6_CPU_H

#include "../common/q6_test.h"
#include "../common/timer.h"

uint64_t tpch_q6(const data* lineitem);
uint64_t tpch_q6_naive(const data* lineitem);
void tpch_selectivity_print(const data* lineitem);

#endif