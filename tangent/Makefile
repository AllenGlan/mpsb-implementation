# File Names
#CFLAGS = -O  -s

USE_HCONLIB = 1

HCONLIB_ROOT = /home/haakon/Documents/Code/C++/HConLib
HCONLIB_INCLUDE = $(HCONLIB_ROOT)/include
HCONLIB_LIB = $(HCONLIB_ROOT)/lib


CFLAGS = -g

LDFLAGS = -lm

ifeq ($(USE_HCONLIB), 1)
	CFLAGS += -I$(HCONLIB_INCLUDE) -DWITH_HCONLIB
	LDFLAGS += -L$(HCONLIB_LIB) -lWinval -lHGraf -lFlatAlg -lX11
endif


OBJS = tangent_test.o tangent.o
EXEC = main


.PHONY: all clean
.INTERMEDIATE: $(OBJS)

all: $(EXEC)

$(EXEC): $(OBJS)
	c++ $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(EXEC)

%.o: %.c*
	c++ $(CFLAGS) -c $<

clean:
	rm *.o $(EXEC)  >/dev/null 2>&1 || true

