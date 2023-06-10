all:
	g++ -O2 -march=native --std=c++11 q6_test_baseline.cpp q6_test_host.cpp common.c -o q6 `dpu-pkg-config --cflags --libs dpu` -g -Wall -lgomp -lm
dpu:
	dpu-upmem-dpurte-clang -O2 -DNR_TASKLETS=16 -o q6_dpu -Wall q6_test_dpu.c common.c
	dpu-upmem-dpurte-clang -O2 -DNR_TASKLETS=16 -o q6_reduce -Wall reduction_dpu.c common.c
clean:
	rm *.o
