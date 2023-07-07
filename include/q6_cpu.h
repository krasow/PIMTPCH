#ifndef Q6_CPU_H 
#define Q6_CPU_H


#include <common/q6.h>
#include <common/timer.h>

uint64_t q6(const lineitem* l_tups);
uint64_t q6_naive(const lineitem* l_tups);
void q6_selectivity_print(const lineitem* l_tups);

#endif