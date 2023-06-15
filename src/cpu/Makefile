CC=clang++
CFLAGS=-O3 -Ofast -march=native --std=c++11 -g -Wall -fopenmp
LDFLAGS=-lomp -lm 

COMMON_HOST=../common/shared_host.cpp
SOURCE_HOST=main.cpp q6_baseline.cpp $(COMMON_HOST)
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
