CC=g++
CFLAGS=-O2 -march=native --std=c++11 -g -Wall
LDFLAGS=-lgomp -lm

COMMON_HOST=../common/shared_host.cpp
SOURCE_HOST=q6_generate_table.cpp $(COMMON_HOST)
BUILDDIR=../../build/data
TARGETS=$(BUILDDIR)/q6

.phony: all
all: $(TARGETS)

$(BUILDDIR)/q6: $(SOURCE_HOST) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(SOURCE_HOST) -o $(BUILDDIR)/q6 $(LDFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
