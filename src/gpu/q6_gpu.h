#ifndef Q6_GPU_H 
#define Q6_GPU_H

#include "../common/q6_test.h"
#include "../common/timer.h"

#define BLOCK_SIZE 32

void q6_gpu(data *tups, uint64_t *sum);
void q6_gpu_setup(data *tups, data *device_tups, uint64_t *host_q6_sum, uint64_t *device_q6_sum);
void q6_gpu_destruct(data *device_tups, uint64_t *host_q6_sum, uint64_t *device_q6_sum);

#endif