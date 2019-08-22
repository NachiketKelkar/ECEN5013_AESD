#include <fcntl.h> 
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>

#define queue_name 		"/my_queue"
#define queue_size		8

typedef struct{
    int is_command;
	char response[30];
}command;

long long timestamp();
command status_update(int , char*);
static void handler(int sig, siginfo_t *si, void *uc);

FILE *logging;
mqd_t myqueue;

int main()
{
	char buffer[100];
	command *data_rec;
	command data_send;
	int ret;
	struct sigaction sigact;

	sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGINT, &sigact, NULL);
	
	printf("Server started\n");
	printf("\n");
	
	struct mq_attr myqueue_attr;
	myqueue_attr.mq_maxmsg = queue_size;
	myqueue_attr.mq_msgsize = sizeof(command);
	
	logging = fopen("IPC_msg.log","w");
	fprintf(logging,"The IPC method being used is message queues");
	fprintf(logging,"\n\nThis is Server process PID - %d\n\n",getpid());
	myqueue = mq_open(queue_name, O_CREAT | O_RDWR, 0666, &myqueue_attr);
	if(myqueue == -1)
	{
		perror("Queue opening error");
		exit(1);
	}
	for(int i=0; i<10; i++){
	ret = mq_receive(myqueue, buffer, sizeof(buffer),0);
	if(ret == -1)
	{
		perror("Receiving error");
		exit(1);
	}
	data_rec = (command*)buffer;
	fprintf(logging,"[%lld usec] Server Received : %d - %s\n",timestamp(),data_rec->is_command,data_rec->response);
	printf("Data received: %d - %s\n",data_rec->is_command,data_rec->response);
	
	data_send = status_update(data_rec->is_command,data_rec->response);
	fprintf(logging,"[%lld usec] Server Sending  : %d - %s\n",timestamp(),data_send.is_command,data_send.response);
	ret = mq_send(myqueue, (char*)&data_send,sizeof(data_send),0);
	if(ret == -1)
	{
		perror("Send error");
		exit(1);
	}
	}
	mq_close(myqueue);
	mq_unlink(queue_name);
	fclose(logging);
	return 0;
}

command status_update(int commanded, char* msg)
{
	command response1;
	if(commanded == 1)
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
		strcpy(response1.response,"You sent message:");
		strcat(response1.response,msg);
	}
	return response1;
}

// credits: https://programmingtoddler.wordpress.com/2014/11/09/c-how-to-get-system-timestamp-in-second-millisecond-and-microsecond/  Below function is referenced from websitef
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
	mq_close(myqueue);
	mq_unlink(queue_name);
	fprintf(logging,"[%lld] Received Termination signal - %d\n\n",timestamp(),sig);
	fclose(logging);
	exit(0);
}
