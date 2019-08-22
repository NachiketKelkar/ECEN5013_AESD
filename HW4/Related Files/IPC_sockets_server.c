#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <stdbool.h>
#include <sys/time.h>
#include <signal.h>
#define PORT 1234

long long timestamp();
static void handler(int sig, siginfo_t *si, void *uc);

FILE *logging;
int server_file;

typedef struct{
	int is_command;
	char response[50];
}send_data;

send_data status_update(int command, char* msg);

int main() 
{ 
    int new_socket; 
    struct sockaddr_in sok_address; 
    int opt = 1; 
    int addrlen = sizeof(sok_address); 
    char buffer[1024] = {0}; 
	struct sigaction sigact;
    
	sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGINT, &sigact, NULL);

	send_data *data_rec,data_send;
	printf("Server started\n\n");
	
	logging = fopen("IPC_sockets.log","w");
	fprintf(logging,"The sockets are used for Inter Process Communication\n");
	fprintf(logging,"\n\nThis is server process PID - %d\n\n",getpid());

    if ((server_file = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    if (setsockopt(server_file, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    sok_address.sin_family = AF_INET; 
    sok_address.sin_addr.s_addr = INADDR_ANY; 
    sok_address.sin_port = htons( PORT ); 
       
    if (bind(server_file, (struct sockaddr *)&sok_address,  
                                 sizeof(sok_address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_file, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_file, (struct sockaddr *)&sok_address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 

	for(int i = 0; i<10; i++)
	{
		read(new_socket, buffer, 1024);
		data_rec = (send_data*)buffer;
		fprintf(logging,"[%lld usec] Server Data received is %d - %s\n",timestamp(),data_rec->is_command,data_rec->response);
		printf("Request received is %d - %s\n",data_rec->is_command,data_rec->response);
		data_send = status_update(data_rec->is_command,data_rec->response); 
		fprintf(logging,"[%lld usec] Server Data sent is     %d - %s\n",timestamp(),data_send.is_command,data_send.response);
		send(new_socket, (char*)&data_send, sizeof(data_send),0);
	}

	fclose(logging);
	close(server_file);
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

send_data status_update(int command, char* msg)
{
	send_data response1;
	char* std_resp = "You sent ";
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
		strcpy(response1.response,"You sent the message - ");
		strcat(response1.response,msg);
	}
	return response1;
}

static void handler(int sig, siginfo_t *si, void *uc)
{
	fprintf(logging,"[%lld usec] Received Termination signal - %d\n\n",timestamp(),sig);
	fclose(logging);
	close(server_file);
	exit(0);
}
