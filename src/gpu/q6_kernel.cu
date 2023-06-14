#ifndef _Q6_KERNEL_H_
#define _Q6_KERNEL_H_

#include <stdio.h>
#include "q6_gpu.h"

////////////////////////////////////////////////////////////////////////////////
//! @param lineitem
////////////////////////////////////////////////////////////////////////////////
__global__ void q6_kernel(data *lineitem, uint64_t *sum)
{
	extern __shared__ uint64_t temp[]; 

	uint64_t temp_sum = 0;

	for (int j=threadIdx.x; j < NUM_TUPLES; j += blockDim.x) {
	
        if (   ((lineitem + j)->l_shipdate >= Q6_DATE1)   // l_shipdate >= date
            && ((lineitem + j)->l_shipdate < Q6_DATE2)    // l_shipdate < date  
            && ((lineitem + j)->l_discount >= Q6_DISCOUNT1)
            && ((lineitem + j)->l_discount <= Q6_DISCOUNT2)
            && ((lineitem + j)->l_quantity < Q6_QUANTITY)) {
            temp_sum += (lineitem + j)->l_extendedprice * (lineitem + j)->l_discount;
        }
        
	}
    // store local into shared global
	temp[threadIdx.x] = temp_sum;

    // do a reduction here with thread 0
    if (threadIdx.x == 0) {
        for (int j=0; j < blockDim.x; j++) {
            *sum += temp[j];
        }

    }
}



uint64_t q6_gpu(data *lineitem, uint64_t *sum) {

    int threadblock_size = 256;
    int num_blocks = (NUM_TUPLES + threadblock_size - 1) / threadblock_size;

    size_t shared_mem = threadblock_size * u_int64_t;
    q6_kernel<<<num_blocks, threadblock_size, shared_mem, nullptr>>>(lineitems, sum);
}

#endif 