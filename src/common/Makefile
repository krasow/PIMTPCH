### Modify here ###
PROJECT=/home/david/PIM_TPCH
TARGETS=lineitem orders customer tables timer

### Leave alone ###
CC=clang++
CFLAGS=-O3 -Ofast -march=native -c -g -Wall -gdwarf-4
LDFLAGS=-I $(PROJECT)/include

BUILD_DIR=$(PROJECT)/build/common

BINS=$(addprefix $(PROJECT)/build/common/, $(TARGETS))
.phony: all 
all: $(BINS) 

$(BUILD_DIR)/%: %.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) --std=c++11 $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/%: %.c | $(BUILD_DIR)
	clang $(CFLAGS) $< -o $@ $(LDFLAGS)


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
