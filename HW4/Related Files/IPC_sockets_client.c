#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#define PORT 1234 
  
long long timestamp();
static void handler(int sig, siginfo_t *si, void *uc);

FILE* logging;
int sok = 0; 

typedef struct{
	int is_command;
	char message[50];
}send_data;

int main() 
{ 
    struct sockaddr_in server_address; 
    char buffer[1024] = {0}; 
	int i;
	size_t data_size;
	struct sigaction sigact;
	int data;
	
	srand(time(0));
	sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGINT, &sigact, NULL);

	char *message[5] = {"Hello", "I am client","LED ON","LED OFF","Bye bye"};

	send_data *data_rec,data_send;

	logging = fopen("IPC_sockets.log","a");
	fprintf(logging,"The sockets are used for Inter Process Communication\n");
	fprintf(logging,"\n\nThis is client process PID - %d\n\n",getpid());

    if ((sok = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket error \n"); 
        return -1; 
    } 
   
    memset(&server_address, '0', sizeof(server_address)); 
   
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(PORT); 

    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0)  
    { 
        printf("\nInvalid address \n"); 
        return -1; 
    } 
   
    if (connect(sok, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

	for(int i=0; i<10; i++){
	data = rand()%5;
	if(data >1 && data <4)
		data_send.is_command = 1;
	else
		data_send.is_command = 0;	
	strcpy(data_send.message,message[data]);
	fprintf(logging,"[%lld usec] Client Data Sent is     %d - %s\n", timestamp(), data_send.is_command, data_send.message);
	send(sok, (char*)&data_send, sizeof(data_send),0);
	read(sok, buffer, 1024);
	data_rec = (send_data*)buffer;
	fprintf(logging,"[%lld usec] Client Data received is %d - %s\n",timestamp(),data_rec->is_command,data_rec->message);
	printf("Update received is %d - %s\n",data_rec->is_command,data_rec->message);
	}
	fclose(logging);
	close(sok);
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
	fprintf(logging,"[%lld usec] Received Termination signal - %d\n\n",timestamp(),sig);
	fclose(logging);
	close(sok);
	exit(0);
}

