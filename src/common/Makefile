CC=clang++
CFLAGS=-O3 -Ofast -march=native --std=c++11 -c -g -Wall

BUILDDIR=../../build/common

TARGETS=$(BUILDDIR)/tables.o $(BUILDDIR)/timer.o $(BUILDDIR)/lineitem.o 

.phony: all 
all: $(TARGETS) 

$(BUILDDIR)/timer.o:: timer/timer.cpp | $(BUILDDIR)
	$(CC) $(CFLAGS) timer/timer.cpp -o $(BUILDDIR)/timer.o 

$(BUILDDIR)/tables.o: tables/tables.cpp  | $(BUILDDIR)
	$(CC) $(CFLAGS) tables/tables.cpp -o $(BUILDDIR)/tables.o 

$(BUILDDIR)/lineitem.o: tables/lineitem.cpp  | $(BUILDDIR)
	$(CC) $(CFLAGS) tables/lineitem.cpp -o $(BUILDDIR)/lineitem.o

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
