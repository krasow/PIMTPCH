// q6_dpu.c 
// upmem dpu binary for TPCH Q6
#include <stdio.h>
#include <string.h>

#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <perfcounter.h>
#include <handshake.h>
#include <barrier.h>
#include <assert.h>

#include <pim.h>
#include <q6.h>


__host q6_dpu_arguments_t DPU_INPUT_ARGUMENTS;
__host uint32_t nb_perf;

// Barrier
BARRIER_INIT(my_barrier, NR_TASKLETS);

int main() {
    perfcounter_config(COUNT_INSTRUCTIONS, true);
    uint16_t tasklet_id = me();

    // Initialize once the cycle counter
    if (tasklet_id == 0) {
        mem_reset(); // Reset the heap
    }
    barrier_wait(&my_barrier);

    uint32_t num_tuples = DPU_INPUT_ARGUMENTS.l_tups_cnt;
    uint32_t total_transfer_input_bytes = DPU_INPUT_ARGUMENTS.transfer_size;

    // Address of the current processing block in MRAM
    uint32_t mram_base_addr = (uint32_t)DPU_MRAM_HEAP_POINTER;

    uint16_t tasklet_output_size = sizeof(__DOUBLE);
    uint32_t dpu_output_size = NR_TASKLETS << find_log2(tasklet_output_size);

    __DOUBLE* out = (__DOUBLE*)mem_alloc(dpu_output_size);
    memset(out, 0, dpu_output_size);

    // Initialize a local cache to store the MRAM block
    lineitem* l_tups = (lineitem*)mem_alloc(sizeof(lineitem));
    char* storage = (char*)mem_alloc(Q6_ACT_SIZE << BLOCK_SIZE_LOG2);

    char* seeker = storage;
    l_tups->l_shipdate = (__DATE*)seeker;
    seeker += BLOCK_SIZE << __DATE_LOG2_SIZE; 

    l_tups->l_discount = (__DOUBLE*)seeker;
    seeker += BLOCK_SIZE << __DOUBLE_LOG2_SIZE;

    l_tups->l_quantity = (__BIGINT*)seeker;
    seeker += BLOCK_SIZE << __BIGINT_LOG2_SIZE;

    l_tups->l_extendedprice = (__DOUBLE*)seeker;
    seeker += BLOCK_SIZE << __DOUBLE_LOG2_SIZE;

    assert(seeker  ==  (storage + (Q6_ACT_SIZE << BLOCK_SIZE_LOG2)));

    // split up total elements to reduce allocated WRAM (max 64KB)
    for (uint32_t block_loc = tasklet_id << BLOCK_SIZE_LOG2; block_loc < num_tuples; block_loc += (NR_TASKLETS << BLOCK_SIZE_LOG2)) {
        // Bound checking for the block
        uint32_t block_elems = (block_loc + BLOCK_SIZE >= num_tuples) ? (num_tuples - block_loc) : BLOCK_SIZE;

        uint32_t total_heap_loc = mram_base_addr;

        mram_read((__mram_ptr void const*)(total_heap_loc + (block_loc << __DATE_LOG2_SIZE)),
            l_tups->l_shipdate, block_elems << __DATE_LOG2_SIZE);
        total_heap_loc += num_tuples << __DATE_LOG2_SIZE;

        mram_read((__mram_ptr void const*)(total_heap_loc + (block_loc << __DOUBLE_LOG2_SIZE)),
            l_tups->l_discount, block_elems << __DOUBLE_LOG2_SIZE);
        total_heap_loc += num_tuples << __DOUBLE_LOG2_SIZE;


        mram_read((__mram_ptr void const*)(total_heap_loc + (block_loc << __BIGINT_LOG2_SIZE)),
            l_tups->l_quantity, block_elems << __BIGINT_LOG2_SIZE);
        total_heap_loc += num_tuples << __BIGINT_LOG2_SIZE;

        mram_read((__mram_ptr void const*)(total_heap_loc + (block_loc << __DOUBLE_LOG2_SIZE)),
            l_tups->l_extendedprice, block_elems << __DOUBLE_LOG2_SIZE);
        total_heap_loc += num_tuples << __DOUBLE_LOG2_SIZE;


        // for (uint16_t j = 0; j < block_elems; j++) {
        //     if (l_tups->l_discount[j] > 10 || 
        //         l_tups->l_discount[j] < 0  ||
        //         l_tups->l_shipdate[j] > (1e30) ||
        //         l_tups->l_quantity[j] > 100 ||
        //         l_tups->l_extendedprice[j] > 1000000     
                
        //     ) 
                
        //     {
        //         printf("l_shipdate %u\n",       l_tups->l_shipdate[j]);
        //         printf("l_discount %lu\n",      l_tups->l_discount[j]);
        //         printf("l_quantity %lu\n",      l_tups->l_quantity[j]);
        //         printf("l_extendedprice %lu\n", l_tups->l_extendedprice[j]);
        //     }
        // }
        

        for (uint16_t j = 0; j < block_elems; j++) {
            if ((l_tups->l_shipdate[j]   >= Q6_DATE1)
                & (l_tups->l_shipdate[j] <  Q6_DATE2)
                & (l_tups->l_discount[j] >= Q6_DISCOUNT1)
                & (l_tups->l_discount[j] <= Q6_DISCOUNT2)
                & (l_tups->l_quantity[j] <  Q6_QUANTITY)) {
                out[tasklet_id] += l_tups->l_extendedprice[j] * l_tups->l_discount[j];
            }
        }
    }

    barrier_wait(&my_barrier);

// #ifdef PRINT
//     printf("output %lu from tasklet %d\n", out[tasklet_id], tasklet_id);
// #endif 


    // Write cache to current MRAM block
    uint32_t offset = mram_base_addr + total_transfer_input_bytes + (tasklet_id << find_log2(tasklet_output_size));
    mram_write(&out[tasklet_id], (__mram_ptr void*)(offset), tasklet_output_size);

    nb_perf = perfcounter_get();
    return 0;
}
