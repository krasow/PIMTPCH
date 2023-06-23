/*
-- TPC-H Query 1

select
        l_returnflag,
        l_linestatus,
        sum(l_quantity) as sum_qty,
        sum(l_extendedprice) as sum_base_price,
        sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,
        sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,
        avg(l_quantity) as avg_qty,
        avg(l_extendedprice) as avg_price,
        avg(l_discount) as avg_disc,
        count(*) as count_order
from
        lineitem
where
        l_shipdate <= date '1998-12-01' - interval '90' day
group by
        l_returnflag,
        l_linestatus
order by
        l_returnflag,
        l_linestatus
*/

#include "q1_cpu.h"

#ifdef __ROW
// optimized row-wise implementation
uint64_t q1(const lineitem* l_tups)
{
	uint64_t out = 0;

#pragma omp parallel for default(shared) reduction(+ : out) 
	for (size_t i = 0; i < NUM_TUPLES; i++) {
		uint64_t match = (l_tups[i].l_shipdate >= Q1_DATE1)
			&& (l_tups[i].l_shipdate < Q1_DATE2)
			&& (l_tups[i].l_discount >= Q1_DISCOUNT1)
			&& (l_tups[i].l_discount <= Q1_DISCOUNT2)
			&& (l_tups[i].l_quantity < Q1_QUANTITY);

		out += match * (l_tups[i].l_extendedprice * l_tups[i].l_discount);

	}
	return out;
}



// naive row-wise implementation
uint64_t q1_naive(const lineitem* l_tups)
{
	uint64_t out = 0;
	for (size_t i = 0; i < NUM_TUPLES; i++) {
		if ((l_tups[i].l_shipdate >= Q1_DATE1)
			&& (l_tups[i].l_shipdate < Q1_DATE2)
			&& (l_tups[i].l_discount >= Q1_DISCOUNT1)
			&& (l_tups[i].l_discount <= Q1_DISCOUNT2)
			&& (l_tups[i].l_quantity < Q1_QUANTITY)) {
			out += (l_tups[i].l_extendedprice * l_tups[i].l_discount);
		}
	}
	return out;
}


// selectivity of the tpch q1 printed
void q1_selectivity_print(const lineitem* l_tups)
{
	// estimate selectivity (non performant)
	uint64_t l_shipdate1_cnt = 0, l_shipdate2_cnt = 0, 
			 l_discount1_cnt = 0, l_discount2_cnt = 0, l_quantity_cnt = 0;

	for (size_t i = 0; i < NUM_TUPLES; i++) {
		if (l_tups[i].l_shipdate >= Q1_DATE1) {
			l_shipdate1_cnt += 1;
			if (l_tups[i].l_shipdate < Q1_DATE2) {
				l_shipdate2_cnt += 1;
				if (l_tups[i].l_discount >= Q1_DISCOUNT1) {
					l_discount1_cnt += 1;
					if (l_tups[i].l_discount >= Q1_DISCOUNT2) {
						l_discount2_cnt += 1;
						if (l_tups[i].l_quantity < Q1_QUANTITY) {
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
uint64_t q1(const lineitem* l_tups)
{
	uint64_t out = 0;

#pragma omp parallel for default(shared) reduction(+ : out) 
	for (size_t i = 0; i < NUM_TUPLES; i++) {
		uint64_t match = (l_tups->l_shipdate[i] >= Q1_DATE1)
			& (l_tups->l_shipdate[i] < Q1_DATE2)
			& (l_tups->l_discount[i] >= Q1_DISCOUNT1)
			& (l_tups->l_discount[i] <= Q1_DISCOUNT2)
			& (l_tups->l_quantity[i] < Q1_QUANTITY);

		out += match * (l_tups->l_extendedprice[i] * l_tups->l_discount[i]);
	}
	return out;
}



// naive column-wise implementation
uint64_t q1_naive(const lineitem* l_tups)
{
	//init
    size_t i;
    hmap_u16_t *out = hmap_u16_init(65536, 48);

    //scan lineitem
    for (i = 0; i < NUM_TUPLES; i++) {
        if (l_tups->l_shipdate[i] <= Q1_DATE1) {
            u16_t key = (l_tups->l_returnflag[i] << 8)
                      | l_tups->l_linestatus[i];
            val_t val = hmap_u16_put(out, key);
            val_i64(val, 0) += 1;
            val_f64(val, 8) += l_tups->l_quantity[i];
            val_f64(val, 16) += l_tups->l_extendedprice[i];
            val_f64(val, 24) += l_tups->l_discount[i];
            f64_t tmp1 = l_tups->l_extendedprice[i]
                       * (1.0 - l_tups->l_discount[i]);
            val_f64(val, 32) += tmp1;
            f64_t tmp2 = tmp1 * (1.0 + l_tups->l_tax[i]);
            val_f64(val, 40) += tmp2;
        }
    }
	return out;
}


// selectivity of the tpch q1 printed
void q1_selectivity_print(const lineitem* l_tups)
{
	// estimate selectivity (non performant)
	uint64_t l_shipdate1_cnt = 0, l_shipdate2_cnt = 0, 
			 l_discount1_cnt = 0, l_discount2_cnt = 0, l_quantity_cnt = 0;

	for (size_t i = 0; i < NUM_TUPLES; i++) {
		if (l_tups->l_shipdate[i] >= Q1_DATE1) {
			l_shipdate1_cnt += 1;
			if (l_tups->l_shipdate[i] < Q1_DATE2) {
				l_shipdate2_cnt += 1;
				if (l_tups->l_discount[i] >= Q1_DISCOUNT1) {
					l_discount1_cnt += 1;
					if (l_tups->l_discount[i] >= Q1_DISCOUNT2) {
						l_discount2_cnt += 1;
						if (l_tups->l_quantity[i] < Q1_QUANTITY) {
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

