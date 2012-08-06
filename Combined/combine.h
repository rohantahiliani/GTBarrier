#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include "mpi.h"

#include <unistd.h>
#include <omp.h>
#include <malloc.h>
#include <string.h>


#define DUMMY -1

typedef struct {
	short parent_id;
	short child_pointers[2];
	short wakeup;
	short have_child[4];
}n_mpi;

typedef struct Node{
	int parentSense;
	int *parentPointer;
	int *childPointer[2];
	int haveChild[4];
	int childNotReady[4];
	int dummy;
}node;

extern void init_mpi();
extern void barrier_mpi();
extern void init_openmp(int sense);
extern void barrier_openmp(int *sense);
extern int childrenArrived(int i, int sense);
