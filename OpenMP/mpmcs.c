#include "mpmcs.h"


//MP Barrier using MCS Algorithm

node *p= NULL;
int num_threads=0;
unsigned long t_sec,t_usec;
struct timeval t_barr1,t_barr2,t_total1,t_total2;
struct timeval t_start[8], t_end[8];

void init(int sense){
	int i=0,j;
	int ind=0;

	for(i=0;i<num_threads;i++){
		for(j=0 ;j<=3;j++){
			if( ((4*i) +(j+1)) < num_threads){
	            p[i].haveChild[j] = sense;
    	        p[i].childNotReady[j] = !sense;
    	     }
	         else{
             	p[i].haveChild[j] = !sense;
             	p[i].childNotReady[j] = sense;
             }
		}
		if(i==0){
			p[i].parentPointer = &p[i].dummy;
			p[i].parentSense=sense;
		}else{
			p[i].parentPointer = &p[(int) floor((i-1)/4)].childNotReady[(i-1) %4];
		}
		ind= (2*i)+1;
		p[i].childPointer[0]= (ind<num_threads)? &p[ind].parentSense:&p[i].dummy;
		ind++;
		p[i].childPointer[1]= (ind<num_threads)? &p[ind].parentSense:&p[i].dummy;
	}

}

void print_time(struct timeval *t1, struct timeval *t2, int id){
	t_sec=(t2->tv_sec-t1->tv_sec);
	t_usec=(t2->tv_usec-t1->tv_usec);
	if(t1->tv_usec>t2->tv_usec) {t_usec=1000000-t1->tv_usec+t2->tv_usec;t_sec-=1;}
	printf("Thread %d = %ld:%ld\n",id,t_sec,t_usec);fflush(stdout);
}

int childrenArrived(int i, int sense){
	int val = 1;
	int j = 0;
	for(j=0;j<=3;j++){
		if(p[i].childNotReady[j] == !sense){
        	val=0;
       		break;
		}
	}
  return val;
}


void barrier(int *sense){
	int i= omp_get_thread_num();
	int j=0;
	//printf("%d is entering Barrier\n", i);
	gettimeofday(&t_start[i],0);
	while((childrenArrived(i, *sense)) != 1){
	}
#pragma omp critical
	{
      for(j=0;j<=3;j++){
         if(*sense){
            p[i].childNotReady[j] = p[i].haveChild[j];
         }
         else{
            p[i].childNotReady[j] = !p[i].haveChild[j];
         }
      }
	}

      *p[i].parentPointer = *sense; // let parents know I am ready

      while((p[i].parentSense) != *sense) // Wait for parent signals wakeup
      {
      }

      *p[i].childPointer[0] = *sense; // signal children in wakeup tree
      *p[i].childPointer[1] = *sense;
		gettimeofday(&t_end[i],0);
	  *sense = !(*sense);
      if(i == 0)
      {
         p[i].parentSense = *sense;
      }

}

int main(int argc, char *argv[]){
	gettimeofday(&t_total1,NULL);
	num_threads = atoi(argv[1]);
	p = (node*)malloc(num_threads*(sizeof(node)));
	int thread_num = -1, priv = 0, pub = 0;
	int sense = 1;
	init(sense);

	gettimeofday(&t_barr1,NULL);
#pragma omp parallel num_threads(num_threads) firstprivate(thread_num, priv) shared(pub) private(sense)
  	{
		sense=1;
    	thread_num = omp_get_thread_num();

   		printf("thread %d => Hello World \n", thread_num);
#pragma omp critical
		{
			priv += thread_num;
			pub += thread_num;
		}

		printf("thread %d => Before Barrier pub= %d\n", thread_num, pub);
		barrier(&sense);
		printf("thread %d => After Barrier final pub= %d\n", thread_num, pub);
		barrier(&sense);
	    printf("thread %d => After Barrier final priv= %d\n", thread_num, priv);

	}
	gettimeofday(&t_barr2,NULL);
	print_time(&t_barr1, &t_barr2,999);

	gettimeofday(&t_total2,NULL);
	print_time(&t_total1, &t_total2,9999);

	int m=0;
	for(m=0;m<num_threads;m++){
		print_time(&t_start[m], &t_end[m], m);
	}

  return 0;
}
