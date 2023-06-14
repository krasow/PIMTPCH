#ifndef Q6_UPMEM_H 
#define Q6_UPMEM_H
#include "../common/q6_test.h"

#define NUM_DPUS        32
#define NUM_TASKLETS    16
#define BLOCK_SIZE_LOG2 4
#define BLOCK_SIZE      (1 << BLOCK_SIZE_LOG2)


typedef struct {
    uint32_t size;
    uint32_t transfer_size;
} dpu_arguments_t;

uint16_t find_log2(uint16_t input);


#endif