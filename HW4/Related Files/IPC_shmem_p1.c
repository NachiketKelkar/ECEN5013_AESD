#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>   
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

#define SHARED_MEMORY_NAME "/shared_memory"
#define SEMAPHORE_1		   "/semaphore1"
#define SEMAPHORE_2		   "/semaphore2"

typedef struct{
	int is_command;
	char command[40];
	}send;

long long timestamp();
static void handler(int sig, siginfo_t *si, void *uc);

FILE* logging;
int shared_mem;

int main()
{
	sem_t *sem1, *sem2;
	send *data_rec,*shared_memory;
	send data_send;
	int data;
	char buffer[100];
	struct sigaction sigact;
	
	char* message[5] = {"Hello","LED ON","LED OFF","Good morning","Bye Bye"};
	srand(time(0));

	sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGINT, &sigact, NULL);

	logging = fopen("IPC_shmem.log","a");
	fprintf(logging,"\nShared memory use for Inter Process Communication");
	fprintf(logging,"\n\nThis is process1 PID - %d\n",getpid());

	shared_mem = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0660);
	if(shared_mem <0)
	{
		perror("shared_mem");
		exit(1);
	}

	sem1 = sem_open(SEMAPHORE_1, O_CREAT, 0660, 0);
	if(sem1 <0)
	{
		perror("sem1: ");
		exit(1);
	}

	sem2 = sem_open(SEMAPHORE_2, O_CREAT, 0660, 0);
	if(sem1 <0)
	{
		perror("sem2: ");
		exit(1);
	}

	int trunc = ftruncate(shared_mem, sizeof(send));
	if(sem1 <0)
	{
		perror("Truncate ");
		exit(1);
	}

	shared_memory = mmap(NULL, sizeof(send), PROT_READ|PROT_WRITE, MAP_SHARED, shared_mem, 0);
	if(data_rec < 0)
	{	
		perror("shared memory: ");
		exit(1);
	}
	
	for(int i=0;i<10;i++)
	{
		data = rand()%5;
		if(data >0 && data < 3)
			data_send.is_command = 1;
		else
			data_send.is_command = 0;
		strcpy(data_send.command, message[data]);
		fprintf(logging,"[%lld usec] Process1 Data Sending:   %d - %s\n",timestamp(), data_send.is_command, data_send.command);
		memcpy(shared_memory, &data_send, sizeof(send));	
		sem_post(sem1);
		sem_wait(sem2);
		memcpy(buffer, (char*)shared_memory,sizeof(send));
		data_rec = (send*)buffer;
		fprintf(logging,"[%lld usec] Process1 Data Receiving: %d - %s\n",timestamp(), data_rec->is_command, data_rec->command);
		printf("Message received by P1 is %d - %s\n", data_rec->is_command,data_rec->command);
	}
	close(shared_mem);
	sem_unlink(SEMAPHORE_1);
	sem_unlink(SEMAPHORE_2);
	return 0;
}

// credits: https://programmingtoddler.wordpress.com/2014/11/09/c-how-to-get-system-timestamp-in-second-millisecond-and-microsecond/  Below function is referenced from website
long long timestamp()
{
  struct timeval timer_usec; 
  long long int timestamp_usec; /* timestamp in microsecond */
  if (!gettimeofday(&timer_usec, NULL)) {
    timestamp_usec = ((long long int) timer_usec.tv_sec) * 1000000ll + 
                        (long long int) timer_usec.tv_usec;
  }
  else {
    timestamp_usec = -1;
	  }
  return timestamp_usec;
}

static void handler(int sig, siginfo_t *si, void *uc)
{
	fprintf(logging,"[%lld] Received Termination signal - %d\n\n",timestamp(),sig);
	fclose(logging);
	close(shared_mem);
	sem_unlink(SEMAPHORE_1);
	sem_unlink(SEMAPHORE_2);
	exit(0);
}
