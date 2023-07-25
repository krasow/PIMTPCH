/*
-- TPC-H Query 3

select
        l_orderkey,
        sum(l_extendedprice * (1 - l_discount)) as revenue,
        o_orderdate,
        o_shippriority
from
        customer,
        orders,
        lineitem
where
        c_mktsegment = 'BUILDING'
        and c_custkey = o_custkey
        and l_orderkey = o_orderkey
        and o_orderdate < date '1995-03-15'
        and l_shipdate > date '1995-03-15'
group by
        l_orderkey,
        o_orderdate,
        o_shippriority
order by
        revenue desc,
        o_orderdate
limit 10
*/
#include <q3.h>
#include <iostream>


#ifdef __COL
DEFINE_HASHTABLE_INSERT(q3_insert, __BIGINT, values*);

__DOUBLE* q3_naive(const lineitem* l_tups, const orders* o_tups, const customer* c_tups)
{
    size_t i = 0;
    struct values* val;

    struct tpch_hashtable* join1 = tpch_create_htable(56000, hash_fn, int_keys_equal_fn); //28k * 2
    struct tpch_hashtable* join2 = tpch_create_htable(268000, hash_fn, int_keys_equal_fn); //134k * 2

    __DOUBLE* out = (__DOUBLE*)calloc(10, sizeof(__DOUBLE));

    //scan customer
    for (i = 0; i < c_tups->elements; i++)
    {
        // c_mktsegment = 'BUILDING'
        // std::cout << GET_STRING(c_tups, c_mktsegment, 3, i) << std::endl;
        if (strncmp((char*)GET_STRING(c_tups, c_mktsegment, 3, i),
            Q3_SEGMENT,
            GET_STRING_SIZE(c_tups, 3)) == 0)
        {
            __BIGINT key = c_tups->c_custkey[i];
            val_malloc(val, 0, 0);
            q3_insert(join1, key, val);
            val = nullptr;
        }
    }

    //scan orders
    for (i = 0; i < o_tups->elements; i++)
    { 
        if (o_tups->o_orderdate[i] < Q3_DATE1) {
            __BIGINT key1 = o_tups->o_custkey[i];
            // and c_custkey = o_custkey
            if (tpch_htable_search(join1, key1)) {
                __BIGINT key2 = o_tups->o_orderkey[i];
                val_malloc(val, 0, 1);
                q3_insert(join2, key2, val);
                val = nullptr;
            }
        }
    }

    //scan lineitem
    for (i = 0; i < l_tups->elements; i++) {
        if (l_tups->l_shipdate[i] > Q3_DATE1) {
            __BIGINT key = l_tups->l_orderkey[i];
        
            // and l_orderkey = o_orderkey
            val = (values*)tpch_htable_search(join2, key);
            if (val != NULL) {
                val_f64(val, 0) += l_tups->l_extendedprice[i]
                    * (10 - l_tups->l_discount[i]);
            }
        }
    }

    //get top 10
    __BIGINT imin = 0;
    __DOUBLE vmin = 0;
    struct tpch_hashtable_iter* iter;

    for (iter = tpch_create_htable_iter(join2); iter->entry; tpch_htable_iter_advance(iter))
    {
        // uint16_t key = (uint16_t)tpch_htable_get_iter_key(iter);
        values* val = (values*)tpch_htable_get_iter_value(iter);

        __DOUBLE tmp = val_f64(val, 0);
        if (tmp > vmin) {
            out[imin] = tmp;
            vmin = tmp;
            for (i = 0; i < 10; i++) {
                if (out[i] < vmin) {
                    vmin = out[i];
                    imin = i;
                }
            }
        }
    }
    
    tpch_free_htable(join1, 0, 0);
    tpch_free_htable(join2, 0, 0);
    return out;
}
#endif

