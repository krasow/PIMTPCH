#ifndef Q1_CPU_H 
#define Q1_CPU_H


#include "../../common/queries/q1.h"
#include "../../common/timer/timer.h"

uint64_t q1(const lineitem* l_tups);
uint64_t q1_naive(const lineitem* l_tups);
void q1_selectivity_print(const lineitem* l_tups);

#endif