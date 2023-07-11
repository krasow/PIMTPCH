#ifndef Q3_CPU_H 
#define Q3_CPU_H


#include <common/q3.h>
#include <common/timer.h>

uint64_t q3(const lineitem* l_tups, const orders* o_tups, const customer* c_tups);
uint64_t q3_naive(const lineitem* l_tups, const orders* o_tups, const customer* c_tups);


#endif