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

#include <q1_cpu.h>


#ifdef __COL
// naive column-wise implementation
// uint64_t q1_naive(const lineitem * l_tups)
// {

// 	size_t i;
// 	hmap_u16_t* out = hmap_u16_init(65536, 48);

// 	//scan lineitem
// 	for (i = 0; i < l_tups->elements; i++) {
// 		if (l_tups->l_shipdate[i] <= Q1_DATE1) {
// 			uint16_t key = (l_tups->l_returnflag[i] << 8)
// 				| l_tups->l_linestatus[i];
// 			val_t val = hmap_u16_put(out, key);
// 			val_i64(val, 0) += 1;
// 			val_f64(val, 8) += l_tups->l_quantity[i];
// 			val_f64(val, 16) += l_tups->l_extendedprice[i];
// 			val_f64(val, 24) += l_tups->l_discount[i];
// 			double tmp1 = l_tups->l_extendedprice[i]
// 				* (1.0 - l_tups->l_discount[i]);
// 			val_f64(val, 32) += tmp1;
// 			double tmp2 = tmp1 * (1.0 + l_tups->l_tax[i]);
// 			val_f64(val, 40) += tmp2;
// 		}
// 	}
// 	return out;

// }
#endif

