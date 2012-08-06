OMPFLAGS = -fopenmp
OMPLIBS = -lgomp -lm

CC = gcc
CPPFLAGS = -g -Wall $(OMPFLAGS)
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

all: mpdissemination mpmcs 

mpmcs: mpmcs.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

mpdissemination: mpdissemination.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o mpdissemination mpmcs
