BUILDDIR=./build
SRCDIR=./src


TESTS := cpu upmem data
.phony: all 

all:
	$(foreach t, $(TESTS), $(MAKE) -C $(SRCDIR)/$(t) ;)

clean:
	$(foreach t, $(TESTS), $(MAKE) clean -C $(SRCDIR)/$(t) ;)
	rm -rf $(BUILDDIR)
