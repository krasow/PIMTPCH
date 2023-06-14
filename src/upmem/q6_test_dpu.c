#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <defs.h>
#include <mram.h>
#include <mram_unaligned.h>
#include <alloc.h>
#include <perfcounter.h>
#include <handshake.h>
#include <barrier.h>
#include <perfcounter.h>

#include "q6_upmem.h"

__host dpu_arguments_t DPU_INPUT_ARGUMENTS;
__host uint32_t nb_perf;

// Barrier
BARRIER_INIT(my_barrier, NUM_TASKLETS);

int main() {
    perfcounter_config(COUNT_INSTRUCTIONS, true);
    uint16_t tasklet_id = me();

    if (tasklet_id == 0) { // Initialize once the cycle counter
        mem_reset(); // Reset the heap
    }
    barrier_wait(&my_barrier);

    uint32_t input_size_dpu_bytes = DPU_INPUT_ARGUMENTS.size; // Input size per DPU in bytes
    uint32_t total_transfer_offset = DPU_INPUT_ARGUMENTS.transfer_size; // Transfer input size per DPU in bytes

    // Address of the current processing block in MRAM
    uint32_t mram_base_addr = (uint32_t)DPU_MRAM_HEAP_POINTER;

    uint16_t elem_size = sizeof(data);
    uint16_t elem_size_log2 = find_log2(elem_size);

    uint16_t tasklet_output_size = sizeof(uint64_t);
    uint16_t tasklet_output_size_log2 = find_log2(tasklet_output_size);

    uint32_t dpu_output_size = NUM_TASKLETS << tasklet_output_size_log2;
    uint32_t input_size_dpu_elems = input_size_dpu_bytes >> elem_size_log2;
    

    // Initialize a local cache to store the MRAM block
    data* lineitem = (data*)mem_alloc(elem_size << BLOCK_SIZE_LOG2);
    uint64_t* out = (uint64_t*)mem_alloc(dpu_output_size);
    // split up total elements to reduce allocated WRAM (max 64KB)
    for (uint32_t i = tasklet_id << BLOCK_SIZE_LOG2; i < input_size_dpu_elems; i += NUM_TASKLETS << BLOCK_SIZE_LOG2) {
        // Bound checking for the block
        uint32_t block_elems = (i + BLOCK_SIZE >= input_size_dpu_elems) ? (input_size_dpu_elems - i) : BLOCK_SIZE;
   
        mram_read((__mram_ptr void const*)(mram_base_addr + i), lineitem, block_elems << elem_size_log2);

        for (uint16_t j = 0; j < block_elems; j++) {
            if (   ((lineitem + j)->l_shipdate >= Q6_DATE1)   // l_shipdate >= date
                && ((lineitem + j)->l_shipdate < Q6_DATE2)    // l_shipdate < date  
                && ((lineitem + j)->l_discount >= Q6_DISCOUNT1)
                && ((lineitem + j)->l_discount <= Q6_DISCOUNT2)
                && ((lineitem + j)->l_quantity < Q6_QUANTITY)) {
                out[tasklet_id] += (lineitem + j)->l_extendedprice * (lineitem + j)->l_discount;
            }
        }
    }

    barrier_wait(&my_barrier);

#ifdef PRINT
    printf("output %lu from tasklet %d\n", out[tasklet_id], tasklet_id);
#endif 


    // Write cache to current MRAM block
    uint32_t offset = mram_base_addr + total_transfer_offset + (tasklet_id << tasklet_output_size_log2);
    mram_write(&out[tasklet_id],(__mram_ptr void*)(offset), tasklet_output_size);

    nb_perf = perfcounter_get();
    return 0;
}
