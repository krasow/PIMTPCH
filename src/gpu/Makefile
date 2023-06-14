CC=nvcc
CFLAGS=-O2 --std=c++11 -g 
LDFLAGS=-lm

COMMON_HOST=../common/shared_host.cpp
SOURCE_HOST=main.cu q6_kernel.cu $(COMMON_HOST)
BUILDDIR=../../build/cpu
TARGETS=$(BUILDDIR)/q6

.phony: all
all: $(TARGETS)

$(BUILDDIR)/q6: $(SOURCE_HOST) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(SOURCE_HOST) -o $(BUILDDIR)/q6 $(LDFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
