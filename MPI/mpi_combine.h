#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include "mpi.h"

#define DUMMY -1

typedef struct {
	short parent_id;
	short child_pointers[2];
	short wakeup;
	short have_child[4];
}node;

extern void init();
extern void barrier();
