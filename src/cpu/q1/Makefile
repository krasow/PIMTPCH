CC=clang++
CFLAGS=-O3 -Ofast -march=native --std=c++11 -g -Wall -fopenmp
LDFLAGS=-lomp -lm 

BUILD_DIR=../../../build

COMMON=$(BUILD_DIR)/common/timer.o $(BUILD_DIR)/common/tables.o $(BUILD_DIR)/common/lineitem.o
SOURCE=main.cpp q1_baseline.cpp

CPU_BUILD_DIR=$(BUILD_DIR)/cpu
TARGETS=$(CPU_BUILD_DIR)/q1

.phony: all
all: $(TARGETS)

$(CPU_BUILD_DIR)/q1: $(SOURCE) | $(CPU_BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCE) $(COMMON) -o $(CPU_BUILD_DIR)/q1 $(LDFLAGS)

$(CPU_BUILD_DIR):
	mkdir -p $(CPU_BUILD_DIR)

clean:
	rm -rf $(CPU_BUILD_DIR)
