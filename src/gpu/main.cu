
#include <string.h>
#include <cuda.h>

#include "q6_gpu.h"

int main(int argc, char *argv[]){ 
    srand(SEED);

	// storing times
	class calc_time gpu_t;

	// get from DATABASE stored
	data* tups = retrieve();
    data* device_tups = NULL;

#ifdef DEBUG
	print_data(tups);
#endif


	uint64_t tpch_q6_sum = 0;
    uint64_t *device_q6_sum = NULL;


	uint32_t tests = (argc == 1) ? 1 : atoi(argv[1]);

    for (uint32_t i = 0; i < tests; i++) {
        cudaMemcpy((void *) device_q6_sum, &tpch_q6_sum, sizeof(uint64_t*), cudaMemcpyHostToDevice);
   
        cudaMalloc((void *) device_tups, NUM_TUPLES * sizeof(data));
        // assuming all tuples fit on GPU all at once
        cudaMemcpy(device_tups, tups, NUM_TUPLES * sizeof(data), cudaMemcpyHostToDevice);

		gpu_t.start();
		q6_gpu(device_tups, device_q6_sum);
		gpu_t.stop();

        cudaDeviceSynchronize();
        cudaMemcpy(&tpch_q6_sum, device_q6_sum, sizeof(uint64_t*), cudaMemcpyDeviceToHost);
	}

	gpu_t.print("CUDA timings", tpch_q6_sum);

	
	free(tups);
	return 0;


}