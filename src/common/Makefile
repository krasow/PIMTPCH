CC=clang++
CFLAGS=-O3 -Ofast -march=native --std=c++11 -g -Wall -c

BUILDDIR=../../build/common
TIMER=timer/timer.cpp
TABLES=tables/lineitem.cpp

TARGETS=$(BUILDDIR)/tables.o $(BUILDDIR)/timer.o 

.phony: all
all: $(TARGETS)

$(BUILDDIR)/tables.o: $(TABLES) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(TABLES) -o $(BUILDDIR)/tables.o

$(BUILDDIR)/timer.o: $(TIMER) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(TIMER) -o $(BUILDDIR)/timer.o 

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
