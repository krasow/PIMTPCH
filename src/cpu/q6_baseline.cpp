/*
-- TPC-H Query 6

select
		sum(l_extendedprice * l_discount) as revenue
from
		lineitem
where
		l_shipdate >= date '1994-01-01'
		and l_shipdate < date '1995-01-01'
		and l_discount between 0.06 - 0.01 and 0.06 + 0.01
		and l_quantity < 24
*/
#include "q6_cpu.h"


#ifdef __ROW
uint64_t tpch_q6(const data* lineitem)
{
	uint64_t out = 0;

#pragma omp parallel for default(shared) reduction(+ : out) 
	for (size_t i = 0; i < NUM_TUPLES; i++) {
		uint64_t match = (lineitem[i].l_shipdate >= Q6_DATE1)
			&& (lineitem[i].l_shipdate < Q6_DATE2)
			&& (lineitem[i].l_discount >= Q6_DISCOUNT1)
			&& (lineitem[i].l_discount <= Q6_DISCOUNT2)
			&& (lineitem[i].l_quantity < Q6_QUANTITY);

		out += match * (lineitem[i].l_extendedprice * lineitem[i].l_discount);

	}
	return out;
}
#endif



#ifdef __COL
uint64_t tpch_q6(const data* lineitem)
{
	uint64_t out = 0;

#pragma omp parallel for default(shared) reduction(+ : out) 
	for (size_t i = 0; i < NUM_TUPLES; i++) {
		uint64_t match = (lineitem->l_shipdate[i] >= Q6_DATE1)
			&& (lineitem->l_shipdate[i] < Q6_DATE2)
			&& (lineitem->l_discount[i] >= Q6_DISCOUNT1)
			&& (lineitem->l_discount[i] <= Q6_DISCOUNT2)
			&& (lineitem->l_quantity[i] < Q6_QUANTITY);

		out += match * (lineitem->l_extendedprice[i] * lineitem->l_discount[i]);
	}
	return out;
}
#endif



// uint64_t tpch_q6_naive(data* lineitem)
// {
// 	uint64_t out = 0;

// 	//scan lineitem
// 	for (size_t i = 0; i < NUM_TUPLES; i++) {
// 		if (((lineitem + i)->l_shipdate >= Q6_DATE1) 
// 			&& ((lineitem + i)->l_shipdate < Q6_DATE2)   
// 			&& ((lineitem + i)->l_discount >= Q6_DISCOUNT1)
// 			&& ((lineitem + i)->l_discount <= Q6_DISCOUNT2)
// 			&& ((lineitem + i)->l_quantity < Q6_QUANTITY)) {	
// 			out += (lineitem + i)->l_extendedprice * (lineitem + i)->l_discount;
// 		}
// 	}
// 	return out;
// }