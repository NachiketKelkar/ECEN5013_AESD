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
#include <time.h>

#define queue_name 		"/my_queue"
#define queue_size		8

typedef struct{
    int is_command;
	char message[30];
}data;

long long timestamp();
static void handler(int sig, siginfo_t *si, void *uc);

FILE *logging;
mqd_t myqueue;

int main()
{
	char buffer[100];	
	
	char *message[5] = {"Hello","LED ON","LED OFF","Good morning","Bye bye"};
	char* message1 = "Hello";
	char* message2 = "LED ON";
	char* message3 = "LED OFF";
	char* message4 = "Good morning";
	char* message5 = "Bye bye";
	srand(time(0));

	int ret, data1;
	struct sigaction sigact;
	
	sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGINT, &sigact, NULL);
	
	data data_send;
	data *data_rec;

	struct mq_attr myqueue_attr;
	myqueue_attr.mq_maxmsg = queue_size;
	myqueue_attr.mq_msgsize = sizeof(data);

	logging = fopen("IPC_msg.log","a");
	fprintf(logging,"The IPC method being used is message queues");
	fprintf(logging,"\n\nThis is Client process PID - %d\n\n",getpid());
	
/* opening the queue for sending and receiving data*/
	myqueue = mq_open(queue_name, O_CREAT | O_RDWR, 0666, &myqueue_attr);
	if(myqueue == (mqd_t)-1)
	{
		perror("Queue opening error");
		exit(1);
	}


	for(int i=0;i<10;i++){
	data1 = rand()%5;
    strcpy(data_send.message,message[data1]);
	if(data1>0 && data1<3)
	    data_send.is_command = 1;
	else
		data_send.is_command = 0;	
	fprintf(logging,"[%lld usec] Client Sending  : %d - %s\n",timestamp(),data_send.is_command,data_send.message);
	ret = mq_send(myqueue, (char*)&data_send, sizeof(data_send),0);
	if(ret == -1)
	{
		perror("Send error");
		exit(1);
	}
	ret = mq_receive(myqueue, buffer, sizeof(buffer),0);
	if(ret == -1)
	{
		perror("Receive error");
		exit(1);
	}
	data_rec = (data*)buffer;
	printf("Data received: %d - %s\n",data_rec->is_command,data_rec->message);
	fprintf(logging,"[%lld usec] Client Received : %d - %s\n",timestamp(),data_rec->is_command,data_rec->message);
	}
	mq_close(myqueue);
	mq_unlink(queue_name);
	fclose(logging);
	return 0;
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
