# File Names
#CFLAGS = -O  -s

USE_HCONLIB = 1

HCONLIB_ROOT = /home/haakon/Documents/Code/C++/HConLib
HCONLIB_INCLUDE = $(HCONLIB_ROOT)/include
HCONLIB_LIB = $(HCONLIB_ROOT)/lib

MPSB_ROOT = ..

CFLAGS = -g -I $(MPSB_ROOT) -I . -I $(MPSB_ROOT)/tangent -std=c++11

LDFLAGS = -lm

ifeq ($(USE_HCONLIB), 1)
	CFLAGS += -I$(HCONLIB_INCLUDE) -DWITH_HCONLIB
	LDFLAGS += -L$(HCONLIB_LIB) -lWinval -lHGraf -lFlatAlg -lX11
endif


OBJS = drawing.o range_tree.o range_tree_test.o common.o tangent.o
EXEC = main


.PHONY: all clean
.INTERMEDIATE: $(OBJS)

all: $(EXEC)

$(EXEC): $(OBJS)
	c++ $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(EXEC)

common.o: 
	c++ $(CFLAGS) -c $(MPSB_ROOT)/common.cpp -o $@
tangent.o:
	c++ $(CFLAGS) -c $(MPSB_ROOT)/tangent/tangent.c -o $@
drawing.o:
	c++ $(CFLAGS) -c $(MPSB_ROOT)/drawing.cpp -o $@

%.o: %.c*
	c++ $(CFLAGS) -c $<

clean:
	rm *.o $(EXEC)  >/dev/null 2>&1 || true

