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
DEFINE_HASHTABLE_INSERT(q1_insert, uint16_t*, values*);

// naive column-wise implementation
void q1_naive(struct tpch_hashtable *out, const lineitem * l_tups)
{
	struct values *val;

	out = tpch_create_htable(65536, hash_fn, int_keys_equal_fn);  // hmap_u16_t* out = hmap_u16_init(65536, 48);
	for (size_t i = 0; i < l_tups->elements; i++) {
		//  l_shipdate <= date '1998-12-01'
		if (l_tups->l_shipdate[i] <= Q1_DATE1) { 
			//  group by  l_returnflag,  l_linestatus
			uint16_t key = (l_tups->l_returnflag[i] << 8) 
				| l_tups->l_linestatus[i];

			val_malloc(val, 1, 5);
			
			val_i64(val, 0) += 1;
			val_f64(val, 0) += l_tups->l_quantity[i];
			val_f64(val, 1) += l_tups->l_extendedprice[i];
			val_f64(val, 2) += l_tups->l_discount[i];
			__DOUBLE tmp1 = l_tups->l_extendedprice[i]
				* (1 - l_tups->l_discount[i]);
			val_f64(val, 3) += tmp1;
			__DOUBLE tmp2 = tmp1 * (1 + l_tups->l_tax[i]);
			val_f64(val, 4) += tmp2;
			q1_insert(out, &key, val);
		}
	}
}
#endif

