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
	char response[40];
	}res;

res status_update(int command, char* msg);
long long timestamp();
static void handler(int sig, siginfo_t *si, void *uc);

FILE* logging;
int shared_mem;

int main()
{
	sem_t *sem1, *sem2;
	res *data_rec,*shared_memory;
	res data_send;
	char buffer[100];
	struct sigaction sigact;

	sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGINT, &sigact, NULL);
	
	printf("Process waiting");

	logging = fopen("IPC_shmem.log","w");
	fprintf(logging,"Shared memory use for Inter Process Communication");
	fprintf(logging,"\n\nThis is process2 PID - %d\n",getpid());

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

	int trunc = ftruncate(shared_mem, sizeof(res));
	if(sem1 <0)
	{
		perror("Truncate ");
		exit(1);
	}

	shared_memory = mmap(NULL, sizeof(res), PROT_READ|PROT_WRITE, MAP_SHARED, shared_mem, 0);
	if(data_rec < 0)
	{	
		perror("shared memory: ");
		exit(1);
	}
	
	for(int i=0;i<10;i++)
	{
		sem_wait(sem1);
		memcpy(buffer, (char*)shared_memory,sizeof(res));
		data_rec = (res*)buffer;
		fprintf(logging,"[%lld usec] Process2 Data Receiving: %d - %s\n",timestamp(), data_rec->is_command, data_rec->response);
		printf("Message received by p2 is %d - %s\n",data_rec->is_command,data_rec->response);
		data_send = status_update(data_rec->is_command,data_rec->response);
		fprintf(logging,"[%lld usec] Process2 Data Sending:   %d - %s\n",timestamp(), data_send.is_command, data_send.response);
		memcpy(shared_memory, &data_send,sizeof(res));
		sem_post(sem2);
	}
	close(shared_mem);
	sem_unlink(SEMAPHORE_1);
	sem_unlink(SEMAPHORE_2);
	return 0;
}

res status_update(int command, char* msg)
{
	res response1;
	if(command == 1)
	{
		if(strcmp(msg,"LED ON")==0)
		{
			response1.is_command = 1;
			strcpy(response1.response,"LED is now ON");
		}
		else
		{
			response1.is_command = 1;
			strcpy(response1.response,"LED is now OFF");
		}
	}
	else
	{
		response1.is_command = 0;
		//strcpy(response1.response,"Message successfully received");
		strcpy(response1.response,"You sent message:");
		strcat(response1.response,msg);
	}
	return response1;
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
