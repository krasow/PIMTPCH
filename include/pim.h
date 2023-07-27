#ifndef PIM_H 
#define PIM_H

#define DPU 1

#ifndef SIMULATOR
#define SIMULATOR 1
#endif


#ifndef NR_TASKLETS
#define NR_TASKLETS 16
#endif

#ifndef NR_DPUS
#define NR_DPUS 32
#endif

#define BLOCK_SIZE_LOG2 6 
#define BLOCK_SIZE      (1 << BLOCK_SIZE_LOG2)



#define __BIGINT_LOG2_SIZE    3
#define __DOUBLE_LOG2_SIZE    3
#define __DATE_LOG2_SIZE      2



#define DPU_REDUCE_BINARY "./build/upmem/common/reduce"


typedef struct {
    uint32_t size;
    uint32_t transfer_size;
} dpu_arguments_t;

#endif
