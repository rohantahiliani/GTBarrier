/*
MPI barrier using the tournament algorithm
*/

#include "mpi_tournament.h"

int my_id, num_processes, logn;
int send_id[1];
int opp_id[1];
short sense;
MPI_Status status;

unsigned long t_sec,t_usec;
struct timeval t_barr1,t_barr2,t_total1,t_total2;

void init(node *ptr){
	int k;
	sense=1;
	for(k=0;k<=logn;k++){
		ptr[k].local_sense=0; //Set local flag to false

		//Set the role depending on which round and node it is
		if((k>0) && (my_id % (int)pow(2,k)==0) && (my_id+pow(2,k-1)<num_processes) && (pow(2,k)<num_processes))
			ptr[k].role=WINNER;
		else if((k>0) && (my_id % (int)pow(2,k)==0) && (my_id+pow(2,k-1)>=num_processes)) ptr[k].role=BYE;
		else if((k>0) && (my_id % (int)pow(2,k)==(int)pow(2,k-1))) ptr[k].role=LOSER;
		else if((k>0) && (my_id==0) && ((int)pow(2,k)>=num_processes)) ptr[k].role=CHAMPION;
		else if(k==0) ptr[k].role=DROPOUT;
		else ptr[k].role=WASTE;

		//Assign opponent id to send and receive messages
		if(ptr[k].role==LOSER) ptr[k].opponent=my_id-(int)pow(2,k-1);
		else if(ptr[k].role==WINNER || ptr[k].role==CHAMPION) ptr[k].opponent=my_id+(int)pow(2,k-1);
		else ptr[k].opponent=WASTE;
	}
}

void print_time(struct timeval *t1, struct timeval *t2){
	t_sec=(t2->tv_sec-t1->tv_sec);
	t_usec=(t2->tv_usec-t1->tv_usec);
	if(t1->tv_usec>t2->tv_usec) {t_usec=1000000-t1->tv_usec+t2->tv_usec;t_sec-=1;}
	printf("Process %d = %ld:%ld\n",my_id,t_sec,t_usec);fflush(stdout);
}

void barrier(node *ptr,int bar_cnt){
	int bcnt=0,round,cont=1;
	while(bcnt++<bar_cnt){//Simulate 4 barriers
		gettimeofday(&t_barr1,NULL);
		round=1;
		send_id[0]=my_id;
		while(cont==1){//Arrival
			switch(ptr[round].role){
				case LOSER:
					printf("%d arriving.\n",my_id);fflush(stdout);
					MPI_Rsend(&send_id, 1, MPI_INT, ptr[round].opponent, 1, MPI_COMM_WORLD);
					MPI_Recv(&opp_id, 1, MPI_INT, ptr[round].opponent, 1, MPI_COMM_WORLD, &status);
					cont=0;
					break;
				case WINNER:
					MPI_Recv(&opp_id, 1, MPI_INT, ptr[round].opponent, 1, MPI_COMM_WORLD, &status);
					break;
				case BYE: break;
				case CHAMPION:
					MPI_Recv(&opp_id, 1, MPI_INT, ptr[round].opponent, 1, MPI_COMM_WORLD, &status);
					MPI_Rsend(&send_id, 1, MPI_INT, ptr[round].opponent, 1, MPI_COMM_WORLD);
					printf("%d waking up %d\n",my_id,ptr[round].opponent);fflush(stdout);
					cont=0;
					break;
				case DROPOUT: break;
			}
			if(cont) round++;
		}
		while(cont==0){//Wakeup
			round--;
			switch(ptr[round].role){
				case LOSER: break;
				case WINNER:
					MPI_Rsend(&send_id, 1, MPI_INT, ptr[round].opponent, 1, MPI_COMM_WORLD);
					printf("%d waking up %d\n",my_id,ptr[round].opponent);fflush(stdout);
					break;
				case BYE: break;
				case CHAMPION: break;
				case DROPOUT: cont=1;
			}
		}
		printf("Barrier %d achieved by process %d.\n",bcnt,my_id);
		gettimeofday(&t_barr2,NULL);
		print_time(&t_barr1, &t_barr2);
	}
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

	logn=(int)(log(num_processes)/log(2));
	node n[logn+1];
	gettimeofday(&t_total1,NULL);
	init(&n);
	barrier(&n,4);
	gettimeofday(&t_total2,NULL);
	printf("Total for 4 barriers for ");
	print_time(&t_total1, &t_total2);
	MPI_Finalize();
	return 0;
}
