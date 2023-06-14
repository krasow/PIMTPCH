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


uint64_t tpch_q6(data* lineitem)
{
	uint64_t out = 0;

	//scan lineitem
	for (size_t i = 0; i < NUM_TUPLES; i++) {
		if (((lineitem + i)->l_shipdate >= Q6_DATE1)  // l_shipdate >= date
			&& ((lineitem + i)->l_shipdate < Q6_DATE2)   // l_shipdate < date
			&& ((lineitem + i)->l_discount >= Q6_DISCOUNT1)
			&& ((lineitem + i)->l_discount <= Q6_DISCOUNT2)
			&& ((lineitem + i)->l_quantity < Q6_QUANTITY)) {
			out += (lineitem + i)->l_extendedprice * (lineitem + i)->l_discount;
		}
	}
	return out;
}


