OMPFLAGS = -fopenmp
OMPLIBS = -lgomp -lm

CC = gcc
CPPFLAGS = -g -Wall $(OMPFLAGS)
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

MPICC = mpicc
MPICH = /usr/lib64/openmpi/1.4-gcc/include
CFLAGS = -I$(MPICH)

all: combine

combine: combine.o
	$(MPICC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o combine
