#include <stdio.h>
#include <unistd.h>
#include <omp.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


typedef struct noded{
	int parentSense;
	int *parentPointer;
	int *childPointer[2];
	int haveChild[4];
	int childNotReady[4];
	int dummy;
}node;

extern void init(int sense);
extern void barrier(int *sense);
extern int childrenArrived(int i, int sense);