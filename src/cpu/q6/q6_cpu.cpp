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
#include <q6.h>
#include <iostream>

#ifdef __ROW
// optimized row-wise implementation
uint64_t q6(const lineitem* l_tups)
{
	uint64_t out = 0;

#pragma omp parallel for default(shared) reduction(+ : out) 
	for (size_t i = 0; i < l_tups->elements; i++) {
		uint64_t match = (l_tups->data[i].l_shipdate >= Q6_DATE1)
			&& (l_tups->data[i].l_shipdate < Q6_DATE2)
			&& (l_tups->data[i].l_discount >= Q6_DISCOUNT1)
			&& (l_tups->data[i].l_discount <= Q6_DISCOUNT2)
			&& (l_tups->data[i].l_quantity < Q6_QUANTITY);

		out += match * (l_tups->data[i].l_extendedprice * l_tups->data[i].l_discount);

	}
	return out;
}



// naive row-wise implementation
uint64_t q6_naive(const lineitem* l_tups)
{
	uint64_t out = 0;
	for (size_t i = 0; i < l_tups->elements; i++) {
		if ((l_tups->data[i].l_shipdate >= Q6_DATE1)
			&& (l_tups->data[i].l_shipdate < Q6_DATE2)
			&& (l_tups->data[i].l_discount >= Q6_DISCOUNT1)
			&& (l_tups->data[i].l_discount <= Q6_DISCOUNT2)
			&& (l_tups->data[i].l_quantity < Q6_QUANTITY)) {
			out += (l_tups->data[i].l_extendedprice * l_tups->data[i].l_discount);
		}
	}
	return out;
}


// selectivity of the tpch q6 printed
void q6_selectivity_print(const lineitem* l_tups)
{
	// estimate selectivity (non performant)
	uint64_t l_shipdate1_cnt = 0, l_shipdate2_cnt = 0,
		l_discount1_cnt = 0, l_discount2_cnt = 0, l_quantity_cnt = 0;

	for (size_t i = 0; i < l_tups->elements; i++) {
		if (l_tups->data[i].l_shipdate >= Q6_DATE1) {
			l_shipdate1_cnt += 1;
			if (l_tups->data[i].l_shipdate < Q6_DATE2) {
				l_shipdate2_cnt += 1;
				if (l_tups->data[i].l_discount >= Q6_DISCOUNT1) {
					l_discount1_cnt += 1;
					if (l_tups->data[i].l_discount >= Q6_DISCOUNT2) {
						l_discount2_cnt += 1;
						if (l_tups->data[i].l_quantity < Q6_QUANTITY) {
							l_quantity_cnt += 1;
						}
					}
				}
			}
		}
	}
	// first number should be largest
	printf("%lu %lu %lu %lu %lu \n",
		l_shipdate1_cnt,
		l_shipdate2_cnt,
		l_discount1_cnt,
		l_discount2_cnt,
		l_quantity_cnt
	);
}
#endif



#ifdef __COL
// optimized column-wise implementation
uint64_t q6(const lineitem* l_tups)
{
	uint64_t out = 0;

#pragma omp parallel for default(shared) reduction(+ : out) 
	for (size_t i = 0; i < l_tups->elements; i++) {
		uint64_t match = (l_tups->l_shipdate[i] >= Q6_DATE1)
			& (l_tups->l_shipdate[i] < Q6_DATE2)
			& (l_tups->l_discount[i] >= Q6_DISCOUNT1)
			& (l_tups->l_discount[i] <= Q6_DISCOUNT2)
			& (l_tups->l_quantity[i] < Q6_QUANTITY);

		out += match * (l_tups->l_extendedprice[i] * l_tups->l_discount[i]);
	}
	return out;
}



// naive column-wise implementation
uint64_t q6_naive(const lineitem* l_tups)
{
	uint64_t out = 0;
	for (size_t i = 0; i < l_tups->elements; i++) {
		if ((l_tups->l_shipdate[i] >= Q6_DATE1)
			&& (l_tups->l_shipdate[i] < Q6_DATE2)
			&& (l_tups->l_discount[i] >= Q6_DISCOUNT1)
			&& (l_tups->l_discount[i] <= Q6_DISCOUNT2)
			&& (l_tups->l_quantity[i] < Q6_QUANTITY)) {
			out += (l_tups->l_extendedprice[i] * l_tups->l_discount[i]);
		}
	}
	return out;
}


// selectivity of the tpch q6 printed
void q6_selectivity_print(const lineitem* l_tups)
{
	// estimate selectivity (non performant)
	uint64_t l_shipdate1_cnt = 0, l_shipdate2_cnt = 0,
		l_discount1_cnt = 0, l_discount2_cnt = 0, l_quantity_cnt = 0;

	for (size_t i = 0; i < l_tups->elements; i++) {
		if (l_tups->l_shipdate[i] >= Q6_DATE1) {
			l_shipdate1_cnt += 1;
			if (l_tups->l_shipdate[i] < Q6_DATE2) {
				l_shipdate2_cnt += 1;
				if (l_tups->l_discount[i] >= Q6_DISCOUNT1) {
					l_discount1_cnt += 1;
					if (l_tups->l_discount[i] >= Q6_DISCOUNT2) {
						l_discount2_cnt += 1;
						if (l_tups->l_quantity[i] < Q6_QUANTITY) {
							l_quantity_cnt += 1;
						}
					}
				}
			}
		}
	}
	// first number should be largest
	printf("%lu %lu %lu %lu %lu \n",
		l_shipdate1_cnt,
		l_shipdate2_cnt,
		l_discount1_cnt,
		l_discount2_cnt,
		l_quantity_cnt
	);
}
#endif

