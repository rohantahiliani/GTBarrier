/*
MPI-OpenMP combined barrier using the MCS algorithm for both
*/

#include "combine.h"

int my_id, num_processes, parent_id;
int send_id[1];
int rec_id[1];
n_mpi n;
MPI_Status status;

node *p= NULL;
int num_threads,bcnt;

unsigned long t_sec,t_usec;
struct timeval t_barr1,t_barr2,t_total1,t_total2;

void print_time(struct timeval *t1, struct timeval *t2){
	t_sec=(t2->tv_sec-t1->tv_sec);
	t_usec=(t2->tv_usec-t1->tv_usec);
	if(t1->tv_usec>t2->tv_usec) {t_usec=1000000-t1->tv_usec+t2->tv_usec;t_sec-=1;}
	printf("Process %d = %ld:%ld\n",my_id,t_sec,t_usec);fflush(stdout);
}

void init_mpi(){
	int k;
	for(k=1;k<=4;k++) n.have_child[k-1]=(4*my_id+k<num_processes)?(4*my_id+k):0;
	n.parent_id=(my_id==0)?DUMMY:floor((my_id-1)/4);
	n.child_pointers[0]=(2*my_id+1 >= num_processes)?DUMMY:2*my_id+1;
	n.child_pointers[1]=(2*my_id+2 >= num_processes)?DUMMY:2*my_id+2;
	n.wakeup=(my_id==0)?DUMMY:((my_id%2==0)?my_id-2:my_id-1)/2;
}

void barrier_mpi(){
	int i;
	send_id[0]=my_id;
	for(i=0;i<4;i++) if(n.have_child[i]>0) MPI_Recv(&rec_id, 1, MPI_INT, n.have_child[i], 1, MPI_COMM_WORLD, &status);
	if(n.parent_id!=DUMMY) MPI_Rsend(&send_id, 1, MPI_INT, n.parent_id, 1, MPI_COMM_WORLD);
	printf("Process %d arrived.\n",my_id);fflush(stdout);
	if(my_id!=0){
		MPI_Recv(&rec_id, 1, MPI_INT, n.wakeup, 1, MPI_COMM_WORLD, &status);
		printf("Process %d waking up process %d.\n",n.wakeup,my_id);fflush(stdout);
	}
	if(n.child_pointers[0]!=DUMMY) MPI_Rsend(&send_id, 1, MPI_INT, n.child_pointers[0], 1, MPI_COMM_WORLD);
	if(n.child_pointers[1]!=DUMMY) MPI_Rsend(&send_id, 1, MPI_INT, n.child_pointers[1], 1, MPI_COMM_WORLD);
	printf("Process %d reached barrier %d.\n",my_id,bcnt);fflush(stdout);
	gettimeofday(&t_barr2,NULL);
	print_time(&t_barr1, &t_barr2);
}

void init_openmp(int sense){
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

void barrier_openmp(int *sense){
	int i= omp_get_thread_num();
	int j=0;

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
	*sense = !(*sense);
      if(i == 0)
      {
         p[i].parentSense = *sense;
      }

}

int main(int argc, char **argv)
{
	gettimeofday(&t_total1,NULL);
	num_threads = 2;
	bcnt=0;
	p = (node*)malloc(num_threads*(sizeof(node)));
	int thread_num = -1, priv = 0, pub = 0;
	int sense = 1;
	init_openmp(sense);

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

	init_mpi();

	while(bcnt++<8){
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
			barrier_openmp(&sense);
			printf("thread %d => After Barrier final pub= %d\n", thread_num, pub);
			barrier_openmp(&sense);
		    	printf("thread %d => After Barrier final priv= %d\n", thread_num, priv);
		}
/*		int m=0;
		for(m=0;m<num_threads;m++){
			print_time(&t_start[m], &t_end[m], m);
		};*/
		barrier_mpi();
	}
	gettimeofday(&t_total2,NULL);
	printf("Total for %d barriers for ",bcnt-1);
	print_time(&t_total1, &t_total2);
	MPI_Finalize();
	return 0;
}

