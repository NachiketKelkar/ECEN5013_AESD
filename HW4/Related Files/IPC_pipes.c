#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/time.h>
#include <signal.h>

long long timestamp();
struct response child_status_update(int command, char* msg);
static void handler(int sig, siginfo_t *si, void *uc);
	
struct response{
	int is_status;
	char* status;
};

FILE* logging;

int pipe_msg_p[2];
int pipe_msg_c[2];

int main()
{
	pid_t child;
	//int pipe_msg_p[2];
	//int pipe_msg_c[2];
	pipe(pipe_msg_p);
	pipe(pipe_msg_c);
	struct sigaction sigact;
	
	struct command{
		int is_command;
		char* message;
	};

	sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGINT, &sigact, NULL);
	
	logging = fopen("pipes.log","w");
	fprintf(logging,"Pipes are used for inter process communication\n");
	child = fork();
	if(child < 0){
		printf("Child failed");	
		return 1;
	}
	else if(child > 0)
	{
		char buffer[30];
		char *message[6] = {"Hello I am parent","LED ON","LED OFF","Good morning","How are you","My last message"};
		/*char *message1 = "Hello I am parent";
		char *message2 = "LED ON";
		char *message3 = "LED OFF";
		char *message4 = "Good morning";
		char *message5 = "How are you";
		char *message6 = "My last message";*/  
		struct response r1, r2;
		struct command msg1, msg2;
		int data;

		srand(time(0));
		close(pipe_msg_p[0]);
		close(pipe_msg_c[1]);

		fprintf(logging,"\n\nThis is Parent process PID - %d\n\n",getpid());

		for(int i=0; i<10; i++){
		data = rand()%6;
		if (data >0 && data <3)
			msg1.is_command = 1;
		else
			msg1.is_command = 0;
		msg1.message = message[data];
		fprintf(logging,"[%lld usec] Parent Sending : %d - %s\n",timestamp(),msg1.is_command, msg1.message);
		write(pipe_msg_p[1], &msg1, sizeof(msg1));		
		read(pipe_msg_c[0], &r1, 30);
		fprintf(logging,"[%lld usec] Parent Received: %d - %s\n",timestamp(),r1.is_status, r1.status);
		}
	}
	else
	{
		char *str1 = "Hello I am child\n";
		char buffer_c[30];
		struct command msg_r,msg_r1;
		struct response r1, r2;
		
		close(pipe_msg_p[1]);
		close(pipe_msg_c[0]);
		
		fprintf(logging,"\n\nThis is child process PID - %d\n\n",getpid());

		for(int i=0; i<10; i++){
			read(pipe_msg_p[0], &msg_r, sizeof(msg_r));
			fprintf(logging,"[%lld usec] Child Received: %d - %s\n",timestamp(), msg_r.is_command,msg_r.message);
			
			r1 = child_status_update(msg_r.is_command, msg_r.message);
			fprintf(logging,"[%lld usec] Child Sending : %d - %s\n",timestamp(), r1.is_status,r1.status);
			write(pipe_msg_c[1], &r1, sizeof(r1));
		}
	}
	close(pipe_msg_p[1]);
	close(pipe_msg_c[0]);
	close(pipe_msg_p[0]);
	close(pipe_msg_c[1]);
	while(1);
	fclose(logging);
	return 0;
}

struct response child_status_update(int command, char* msg)
{
	struct response response1;
	if(command == 1)
	{
		if(strcmp(msg,"LED ON")==0)
		{
			response1.is_status = 1;
			response1.status = "LED is now ON";
		}
		else
		{
			response1.is_status = 1;
			response1.status = "LED is now OFF";
		}
	}
	else
	{
		response1.is_status = 0;
		response1.status = "Message successfully received";
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
	fprintf(logging,"[%lld] Received Termination signal - %d\n\n",timestamp(),sig);
	fclose(logging);
	close(pipe_msg_p[1]);
	close(pipe_msg_c[0]);
	close(pipe_msg_p[0]);
	close(pipe_msg_c[1]);
	exit(0);
}
