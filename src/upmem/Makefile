CC=g++
CFLAGS=-O2 -march=native --std=c++11 -g -Wall
LDFLAGS=-lgomp -lm

COMMON_HOST=../common/shared_host.cpp
SOURCE_HOST=main.cpp $(COMMON_HOST)
DPU_PKG=`dpu-pkg-config --cflags --libs dpu`
TASKLETS=16

BUILDDIR=../../build/upmem
TARGETS=$(BUILDDIR)/q6 $(BUILDDIR)/q6_dpu $(BUILDDIR)/q6_reduce 

.phony: all
all: $(TARGETS)

$(BUILDDIR)/q6: $(SOURCE_HOST) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(SOURCE_HOST) -o $(BUILDDIR)/q6 $(DPU_PKG) $(LDFLAGS)

$(BUILDDIR)/q6_dpu: q6_test_dpu.c helper.c  | $(BUILDDIR)
	dpu-upmem-dpurte-clang -O2 -Wall -DNR_TASKLETS=$(TASKLETS) q6_test_dpu.c helper.c -o $(BUILDDIR)/q6_dpu 

$(BUILDDIR)/q6_reduce: reduction_dpu.c helper.c | $(BUILDDIR)
	dpu-upmem-dpurte-clang -O2 -Wall -DNR_TASKLETS==$(TASKLETS) reduction_dpu.c helper.c -o $(BUILDDIR)/q6_reduce

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
