#ifndef Q1_CPU_H 
#define Q1_CPU_H


#include <common/q1.h>
#include <common/timer.h>

uint64_t q1(const lineitem* l_tups);
uint64_t q1_naive(const lineitem* l_tups);

#endif