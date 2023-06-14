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
        printf("%d\n", (lineitem + j)->l_shipdate);
	
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
    // *sum = 132;
}

void q6_gpu_setup(data *tups, data *device_tups, uint64_t *host_q6_sum, uint64_t *device_q6_sum) {
    size_t tableSize = NUM_TUPLES * sizeof(data);
    cudaMemcpy((void *) device_q6_sum, host_q6_sum, sizeof(uint64_t*), cudaMemcpyHostToDevice);
    cudaMalloc((void **) &device_tups, tableSize);
    // assuming all tuples fit on GPU all at once
    cudaMemcpy((void *) device_tups, tups, tableSize, cudaMemcpyHostToDevice);
}

void q6_gpu_destruct(data *device_tups, uint64_t *host_q6_sum, uint64_t *device_q6_sum) {
    cudaDeviceSynchronize();
    cudaMemcpy((void *)host_q6_sum, device_q6_sum, sizeof(uint64_t*), cudaMemcpyDeviceToHost);
    cudaFree(device_tups);
}

void q6_gpu(data *tups, uint64_t *sum) {
    int threadblock_size = BLOCK_SIZE*BLOCK_SIZE;
    int num_blocks = (NUM_TUPLES + threadblock_size - 1) / threadblock_size;

    size_t shared_mem = threadblock_size * sizeof(uint64_t);
    q6_kernel<<<num_blocks, threadblock_size, shared_mem, nullptr>>>(tups, sum);
    // printf("%d\n", sum);
}

#endif 