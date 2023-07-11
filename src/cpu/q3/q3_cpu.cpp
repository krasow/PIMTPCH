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
#include <q3_cpu.h>


#ifdef __COL
uint64_t q3_naive(const lineitem* l_tups, const orders* o_tups, const customer* c_tups)
{
    size_t i = 0;
    // hmap_id_t *join1 = hmap_id_init(56000, 0); //28k * 2
    // hmap_id_t *join2 = hmap_id_init(268000, sizeof(uint64_t)); //134k * 2
    // double *out = calloc(10, sizeof(*out));

    // //scan customer
    // for (i = 0; i < customer->elements; i++)
    //     if (customer->c_mktsegment[i] == Q3_SEGMENT) {
    //         id_t key = customer->c_custkey[i];
    //         hmap_id_put(join1, key);
    //     }

    // //scan orders
    // for (i = 0; i < orders->elements; i++)
    //     if (orders->o_orderdate[i] < Q3_DATE) {
    //         id_t key1 = orders->o_custkey[i];
    //         if (hmap_id_in(join1, key1)) {
    //             id_t key2 = orders->o_orderkey[i];
    //             hmap_id_put(join2, key2);
    //         }
    //     }

    // //scan lineitem
    // for (i = 0; i < lineitem->elements; i++)
    //     if (lineitem->l_shipdate[i] > Q3_DATE) {
    //         id_t key = lineitem->l_orderkey[i];
    //         val_t val = hmap_id_get(join2, key);
    //         if (val != NULL)
    //             val_f64(val, 0) += lineitem->l_extendedprice[i]
    //                              * (1.0 - lineitem->l_discount[i]);
    //     }

    // //get top 10
    // size_t imin = 0;
    // double vmin = 0;
    // itr_id_t *itr = hmap_id_itr(join2);
    // while (hmap_id_next(itr)) {
    //     double tmp = val_f64(itr->val, 0);
    //     if (tmp > vmin) {
    //         out[imin] = tmp;
    //         vmin = tmp;
    //         for (i = 0; i < 10; i++)
    //             if (out[i] < vmin) {
    //                 vmin = out[i];
    //                 imin = i;
    //             }
    //     }
    // }
    return i;
}
#endif

