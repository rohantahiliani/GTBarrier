#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include "mpi.h"

#define WINNER 0
#define LOSER 1
#define BYE 2
#define CHAMPION 3
#define DROPOUT 4
#define WASTE -1

typedef struct {
	short role;
	short opponent;
	short local_sense;
}node;

extern void init();
extern void barrier();

