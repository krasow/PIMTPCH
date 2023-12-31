### Modify here ###
TASKLETS=16
DPUS=32
SIM=1
QUERIES := q6
### Leave alone ###

DEFINES=-DNR_TASKLETS=$(TASKLETS) -DNR_DPUS=$(DPUS) -DSIMULATOR=$(SIM)

### setup CPU query binaries ###
CC=g++
CFLAGS=-O3 -Ofast -march=native --std=c++11 -g -Wall -Wno-deprecated 
LDFLAGS=-lm  -I $(PROJECT)/include 
DPU_PKG=`dpu-pkg-config --cflags --libs dpu`
HMAP=$(PROJECT)/src/common/hmap.c
LOG2=$(PROJECT)/src/common/log2.c


BUILD_DIR=$(PROJECT)/build/upmem

COMMON_OBJ=lineitem tables timer log2
COMMON=$(addprefix $(PROJECT)/build/common/, $(COMMON_OBJ))

BINS=$(addprefix $(BUILD_DIR)/, $(QUERIES))
.phony: all 
all: $(BINS) 

$(BUILD_DIR)/%: %/main.cpp | $(BUILD_DIR) $(BUILD_DIR)/%_dpu GENERAL
	$(CC) $(CFLAGS) $(DEFINES) $< -o $@ $(HMAP) $(COMMON) $(DPU_PKG) $(LDFLAGS)
### end of CPU query binaries ###


### setup DPU binaries ####
COMMON_DPU_HELPERS=$(LOG2) $(HMAP)
CC_DPU=dpu-upmem-dpurte-clang
CFLAGS_DPU=-O2 -Wall $(DEFINES)
LDFLAGS_DPU=-I $(PROJECT)/include
DPU_COMPILE_COMMAND=$(CC_DPU) $(CFLAGS_DPU) $(COMMON_DPU_HELPERS) $< -o $@ $(LDFLAGS_DPU)

### DPU query binaries ###

$(BUILD_DIR)/q6_dpu: q6/q6_dpu.c 
	$(DPU_COMPILE_COMMAND)

$(BUILD_DIR)/q1_dpu: q1/q1_dpu.c 
	$(DPU_COMPILE_COMMAND)


### end of DPU query binaries ###


### GENERAL DPU binaries ###
GEN_DPU=reduce
GEN=$(addprefix $(BUILD_DIR)/common/, $(GEN_DPU))
GENERAL: $(GEN)

$(BUILD_DIR)/common/%: %.c | $(BUILD_DIR)/common
	$(DPU_COMPILE_COMMAND)
### end of GENERAL DPU binaries ###

### end of DPU setup ###

$(BUILD_DIR)/common:
	mkdir -p $(BUILD_DIR)/common

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
