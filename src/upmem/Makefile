# modify this
QUERIES := q6

BUILDDIR=./build
.phony: all 

all:
	$(foreach t, $(QUERIES), $(MAKE) -C ./$(t) ;)

clean:
	$(foreach t, $(QUERIES), $(MAKE) clean -C ./$(t) ;)
	rm -rf $(BUILDDIR)
