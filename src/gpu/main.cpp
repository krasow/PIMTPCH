
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


	uint64_t host_q6_sum = 0;
    uint64_t *device_q6_sum = NULL;


	uint32_t tests = (argc == 1) ? 1 : atoi(argv[1]);

    for (uint32_t i = 0; i < tests; i++) {

		q6_gpu_setup(tups, device_tups, &host_q6_sum, device_q6_sum);

		gpu_t.start();
		q6_gpu(device_tups, device_q6_sum);
		gpu_t.stop();

		q6_gpu_destruct(device_tups, &host_q6_sum, device_q6_sum);
	}
	
	gpu_t.print("CUDA timings", host_q6_sum);

	
	free(tups);
	return 0;


}