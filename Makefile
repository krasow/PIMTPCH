# modify this
TESTS := cpu


BUILDDIR=./build
SRCDIR=./src
COMMON := common
.phony: all 

all:
	$(MAKE) -C $(SRCDIR)/$(COMMON)
	$(foreach t, $(TESTS), $(MAKE) -C $(SRCDIR)/$(t) ;)

clean:
	$(foreach t, $(COMMON) $(TESTS), $(MAKE) clean -C $(SRCDIR)/$(t) ;)
	rm -rf $(BUILDDIR)
