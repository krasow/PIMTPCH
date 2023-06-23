CC=g++
CFLAGS=-O2 -march=native --std=c++11 -g -Wall
LDFLAGS=-lgomp -lm

BUILD_DIR=../../../build

COMMON=$(BUILD_DIR)/common/timer.o $(BUILD_DIR)/common/tables.o $(BUILD_DIR)/common/lineitem.o
SOURCE=main.cpp
DPU_PKG=`dpu-pkg-config --cflags --libs dpu`
TASKLETS=16

UPMEM_BUILD_DIR=$(BUILD_DIR)/upmem
TARGETS=$(UPMEM_BUILD_DIR)/q6 $(UPMEM_BUILD_DIR)/q6_dpu $(UPMEM_BUILD_DIR)/q6_reduce 

.phony: all
all: $(TARGETS)

$(UPMEM_BUILD_DIR)/q6: $(SOURCE) | $(UPMEM_BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCE) $(COMMON) -o $(UPMEM_BUILD_DIR)/q6 $(DPU_PKG) $(LDFLAGS)

$(UPMEM_BUILD_DIR)/q6_dpu: q6_test_dpu.c ../helper.c  | $(UPMEM_BUILD_DIR)
	dpu-upmem-dpurte-clang -O2 -Wall -DNR_TASKLETS=$(TASKLETS) q6_test_dpu.c ../helper.c -o $(UPMEM_BUILD_DIR)/q6_dpu 

$(UPMEM_BUILD_DIR)/q6_reduce: reduction_dpu.c ../helper.c | $(UPMEM_BUILD_DIR)
	dpu-upmem-dpurte-clang -O2 -Wall -DNR_TASKLETS=$(TASKLETS) reduction_dpu.c ../helper.c -o $(UPMEM_BUILD_DIR)/q6_reduce

$(UPMEM_BUILD_DIR):
	mkdir -p $(UPMEM_BUILD_DIR)

clean:
	rm -rf $(UPMEM_BUILD_DIR)
