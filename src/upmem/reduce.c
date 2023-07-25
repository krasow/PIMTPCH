// reduce.c 
// upmem dpu binary for reductions
#include <stdio.h>
#include <string.h>

#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <perfcounter.h>
#include <handshake.h>
#include <barrier.h>

#include <pim.h>
#include <tpch.h>

__host dpu_arguments_t DPU_INPUT_ARGUMENTS;
__host uint32_t nb_perf;

// Barrier
BARRIER_INIT(my_barrier, NR_TASKLETS);

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

    // setup elements
    uint16_t elem_size = sizeof(uint64_t);
    uint16_t elem_size_log2 = find_log2(elem_size);
    uint32_t total_array_elements = input_size_dpu_bytes >> elem_size_log2;


    uint64_t* output = (uint64_t*)mem_alloc(NR_TASKLETS << elem_size_log2);
    uint64_t* array = (uint64_t*)mem_alloc(BLOCK_SIZE << elem_size_log2);
    output[tasklet_id] = 0;


    // split up total elements to reduce allocated WRAM (max 64KB)
    for (uint32_t i = tasklet_id << BLOCK_SIZE_LOG2; i < total_array_elements; i += NR_TASKLETS << BLOCK_SIZE_LOG2) {
        // Bound checking for the block
        uint32_t block_elems = (i + BLOCK_SIZE >= total_array_elements) ? (total_array_elements - i) : BLOCK_SIZE;

        mram_read((__mram_ptr void const*)(mram_base_addr + i), array, block_elems << elem_size_log2);

        for (uint16_t j = 0; j < block_elems; j++) {
            // Reduction in each tasklet on local wram
            output[tasklet_id] += array[j];
        }
    }
#ifdef PRINT
    printf("res output %lu from tasklet %d\n", output[tasklet_id], tasklet_id);
#endif 

    // write local array to global mram
    mram_write(&output[tasklet_id], (__mram_ptr void*)(mram_base_addr + tasklet_id * elem_size), elem_size);
    barrier_wait(&my_barrier);

    uint64_t sum = 0;
    // sum up the remaining elements where N = NR_TASKLETS
    // reduces intra DPU communication by having one tasklet perform all work
    if (tasklet_id == 0) {
        mram_read((__mram_ptr void const*)(mram_base_addr), output, NR_TASKLETS << elem_size_log2);
        for (unsigned int i = 0; i < NR_TASKLETS; i++) {
            sum += output[i];
        }

#ifdef PRINT
        printf("sum: %lu\n", sum);
#endif

        mram_write(&sum, (__mram_ptr void*)(mram_base_addr + total_transfer_offset), elem_size);
    }
    nb_perf = perfcounter_get();
    return 0;
}