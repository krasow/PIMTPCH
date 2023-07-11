#ifndef Q1_CPU_H 
#define Q1_CPU_H


#include <common/q1.h>
#include <common/timer.h>

void q1(struct tpch_hashtable **out, const lineitem * l_tups);
void q1_naive(struct tpch_hashtable **out, const lineitem * l_tups);

#endif