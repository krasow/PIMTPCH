### Modify here ###
TESTS := cpu upmem
PROJECT := /home/david/PIM_TPCH
QUERIES := q6 q3 q1
### Leave alone ###

BUILD_DIR := ./build
SRCDIR := ./src
COMMON := common

.phony: all 
all: | $(BUILD_DIR)
	$(MAKE) PROJECT=$(PROJECT) -C $(SRCDIR)/$(COMMON)
	$(foreach t, $(TESTS), $(MAKE) PROJECT=$(PROJECT) -C $(SRCDIR)/$(t) ;)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(foreach t, $(COMMON) $(TESTS), $(MAKE) clean -C $(SRCDIR)/$(t) ;)
	rm -rf $(BUILD_DIR)
