#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


int **p;
int num_tds=0;
int num_rounds = 0;
unsigned long t_sec,t_usec;
struct timeval t_barr1,t_barr2,t_total1,t_total2;
struct timeval t_start[8], t_end[8];

double findlog2(int a){
	return (log(a)/log(2));
}

void print_time(struct timeval *t1, struct timeval *t2, int id){
	t_sec=(t2->tv_sec-t1->tv_sec);
	t_usec=(t2->tv_usec-t1->tv_usec);
	if(t1->tv_usec>t2->tv_usec) {t_usec=1000000-t1->tv_usec+t2->tv_usec;t_sec-=1;}
	printf("Thread %d = %ld:%ld\n",id,t_sec,t_usec);fflush(stdout);
}

void barrier(int *sense){
    int i =0, index = 0;
    int t = omp_get_thread_num();
    for(i=0;i<num_rounds;i++){
		gettimeofday(&t_start[t],0);
        index = (t+(int)pow(2,i))% num_tds;
        p[index][i] = *sense;
        while(p[t][i] != *sense){
        }
		gettimeofday(&t_end[t],0);
    }
    *sense = ~(*sense);
}

int main(int argc, char **argv){
	gettimeofday(&t_total1,NULL);
	int thread_num = -1, priv = 0, pub = 0;
	int i =0;
	num_tds = atoi(argv[1]);
	num_rounds = (int) ceil(findlog2(num_tds));

	p = (int **) malloc(num_tds*sizeof(int*));
	for(i =0 ;i< num_tds;i++){
	    p[i] = malloc(num_rounds * sizeof(int));
	}
	int sense;
	gettimeofday(&t_barr1,NULL);
#pragma omp parallel num_threads(num_tds) firstprivate(thread_num, priv) private(sense) shared(pub)
    {
		thread_num = omp_get_thread_num();
	    printf("thread %d: Hello World \n", thread_num);
    	sense =1;
#pragma omp critical
    {
   	   priv += thread_num;
   	   pub += thread_num;
    }
    printf("thread %d: before barrier pub is = %d\n", thread_num, pub);
    barrier(&sense);
    printf("thread %d: After barrier pub is = %d\n", thread_num, pub);
    barrier(&sense);
    printf("thread %d: After barrier priv is = %d\n", thread_num, priv);
   }
		gettimeofday(&t_barr2,NULL);
	print_time(&t_barr1, &t_barr2,999);

	gettimeofday(&t_total2,NULL);
	print_time(&t_total1, &t_total2,9999);

	int m=0;
	for(m=0;m<num_tds;m++){
		print_time(&t_start[m], &t_end[m], m);
	}
  return 0;
}

/*int main(int argc, char **argv){
	gettimeofday(&t_total1,NULL);
	int thread_num=-1, priv=0, pub=0;
	int i=0;
	num_tds= atoi(argv[1]);
	num_rounds= (int) ceil(findLog(num_tds));

	p = (int **)malloc(num_tds*sizeof(int*));
	for(i=0; i<num_tds; i++){
		p[i]= malloc(num_rounds * sizeof(int));
	}
	int sense;
#pragma omp parallel num_threads(num_tds) firstprivate(thread_num, priv) private(sense) shared(pub)
{
    	thread_num = omp_get_thread_num();
    	printf("thread %d=> Hello World \n", thread_num);
    	sense =1;
	#pragma omp critical
	{
				priv += thread_num;
				pub += thread_num;
	}
	    printf("thread %d=> Before Barrier pub= %d\n", thread_num, pub);
		barrier(&sense);
		printf("thread %d=> After Barrier final pub= %d\n", thread_num, pub);
		barrier(&sense);
	    printf("thread %d=> After Barrier final priv= %d\n", thread_num, priv);
}

	gettimeofday(&t_barr2,NULL);
	print_time(&t_barr1, &t_barr2,999);

	gettimeofday(&t_total2,NULL);
	print_time(&t_total1, &t_total2,9999);

	int m=0;
	for(m=0;m<num_tds;m++){
		print_time(&t_start[m], &t_end[m], m);
	}

	return 0;
}*/

