### Modify here ###
PROJECT=/home/david/PIM_TPCH
QUERIES := q1 q3 q6 
### Leave alone ###

CC=clang++
CFLAGS=-O3 -Ofast -march=native --std=c++11 -g -Wall -fopenmp
LDFLAGS=-lomp -lm  -I $(PROJECT)/include 

BUILD_DIR=$(PROJECT)/build/cpu

COMMON_OBJ=lineitem orders customer tables timer hmap
COMMON=$(addprefix $(PROJECT)/build/common/, $(COMMON_OBJ))

BINS=$(addprefix $(BUILD_DIR)/, $(QUERIES))
.phony: all 
all: $(BINS) 

$(BUILD_DIR)/%: %/main.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(@F)/$(@F)_cpu.cpp $< -o $@  $(COMMON) $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)