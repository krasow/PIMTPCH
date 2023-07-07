### Modify here ###
TESTS := cpu upmem
### Leave alone ###

BUILD_DIR=./build
SRCDIR=./src
COMMON := common

.phony: all 
all: | $(BUILD_DIR)
	$(MAKE) -C $(SRCDIR)/$(COMMON)
	$(foreach t, $(TESTS), $(MAKE) -C $(SRCDIR)/$(t) ;)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(foreach t, $(COMMON) $(TESTS), $(MAKE) clean -C $(SRCDIR)/$(t) ;)
	rm -rf $(BUILD_DIR)
