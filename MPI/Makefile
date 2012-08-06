OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g -Wall
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

MPICC = mpicc
MPICH = /usr/lib64/openmpi/1.4-gcc/include
CFLAGS = -I$(MPICH)

all: mpi_tournament mpi_mcs

mpi_tournament: mpi_tournament.o
	$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

mpi_mcs: mpi_mcs.o
	$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS)

clean:
	rm -f *.o mpi_tournament mpi_mcs
