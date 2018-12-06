# File Names
#CFLAGS = -O  -s

USE_HCONLIB = 1

HCONLIB_ROOT = /home/haakon/Documents/Code/C++/HConLib
HCONLIB_INCLUDE = $(HCONLIB_ROOT)/include
HCONLIB_LIB = $(HCONLIB_ROOT)/lib

MPSB_ROOT = ..

CFLAGS = -g -I $(MPSB_ROOT) -I .

LDFLAGS = -lm

ifeq ($(USE_HCONLIB), 1)
	CFLAGS += -I$(HCONLIB_INCLUDE) -DWITH_HCONLIB
	LDFLAGS += -L$(HCONLIB_LIB) -lWinval -lHGraf -lFlatAlg -lX11
endif


OBJS = outer_triangle_partition.o test_triangle_partition.o common.o
EXEC = main


.PHONY: all clean
.INTERMEDIATE: $(OBJS)

all: $(EXEC)

$(EXEC): $(OBJS)
	c++ $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(EXEC)

common.o: 
	c++ $(CFLAGS) -c $(MPSB_ROOT)/common.cpp -o $@
%.o: %.c*
	c++ $(CFLAGS) -c $<

clean:
	rm *.o $(EXEC)  >/dev/null 2>&1 || true

