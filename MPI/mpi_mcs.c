/*
MPI barrier using the MCS algorithm
*/

#include "mpi_mcs.h"

int my_id, num_processes, parent_id;
int send_id[1];
int rec_id[1];
node n;
MPI_Status status;
unsigned long t_sec,t_usec;
struct timeval t_barr1,t_barr2,t_total1,t_total2;

void init(){
	int k;
	for(k=1;k<=4;k++) n.have_child[k-1]=(4*my_id+k<num_processes)?(4*my_id+k):0;
	n.parent_id=(my_id==0)?DUMMY:floor((my_id-1)/4);
	n.child_pointers[0]=(2*my_id+1 >= num_processes)?DUMMY:2*my_id+1;
	n.child_pointers[1]=(2*my_id+2 >= num_processes)?DUMMY:2*my_id+2;
	n.wakeup=(my_id==0)?DUMMY:((my_id%2==0)?my_id-2:my_id-1)/2;
}

void print_time(struct timeval *t1, struct timeval *t2){
	t_sec=(t2->tv_sec-t1->tv_sec);
	t_usec=(t2->tv_usec-t1->tv_usec);
	if(t1->tv_usec>t2->tv_usec) {t_usec=1000000-t1->tv_usec+t2->tv_usec;t_sec-=1;}
	printf("Process %d = %ld:%ld\n",my_id,t_sec,t_usec);fflush(stdout);
}

void barrier(int bar_cnt){
	int bcnt=0,i;
	send_id[0]=my_id;
	while(bcnt++<bar_cnt){//Simulate 4 barriers
		gettimeofday(&t_barr1,NULL);
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
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	gettimeofday(&t_total1,NULL);
	init();
	barrier(4);
	gettimeofday(&t_total2,NULL);
	printf("Total for 4 barriers for ");
	print_time(&t_total1, &t_total2);
	MPI_Finalize();
	return 0;
}

