#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <signal.h>
#include <ctype.h>

/* Function declarations */
static void *child1_funct(void *);
static void *child2_funct(void *);
static void handler(int sig, siginfo_t *si, void *uc);
void add_new_member(char*,int);
void print_list();
void timestamp();
long long timestamp1();

int kill_msg = 0;
int print_stat = 0;

FILE *logging;

struct reporting
{
	char alphabet;
	int count;
	struct reporting *next;
};

struct reporting *report = NULL,			// use for adding to the list
				 *head   = NULL; 			// use for indicating the head of the list

timer_t timerid;

int main()
{
/* Local variables */
    pthread_t child1,child2;
    struct sigaction sigact;
	
	struct pthreads_sub
	{
    	char* log_file_name;
	};

	struct pthreads_sub *log_file;
/* Allocationg memory to the variables */
    log_file = (struct pthreads_sub *)malloc(sizeof(struct pthreads_sub));
    log_file->log_file_name = (char*)malloc(20);

    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = handler;
    int success1 = sigaction(SIGRTMIN, &sigact, NULL);
    success1 = sigaction(SIGUSR1, &sigact, NULL);
    success1 = sigaction(SIGUSR2, &sigact, NULL);
    sigemptyset(&sigact.sa_mask);
    printf("Success1 = %d\n", success1);

    printf("The parent pid is %d",getpid());
    printf("\nEnter the name of the log file to use: ");
    scanf("%s",log_file->log_file_name);
    strcat(log_file->log_file_name,".log");                                 // Appending the file format to the file
    printf("\nThe name of the log file is %s",log_file->log_file_name);
    logging = fopen(log_file->log_file_name,"w");                           // Create the log file
    if(logging == NULL)
        printf("\nFile not created");
    else
    {
        printf("\nFile created");
        fprintf(logging,">>>[%lld usec] In the Main thread POSIX thread id = %ld, LINUX thread id = %ld <<<\n",timestamp1(),pthread_self(),syscall(SYS_gettid));
        fclose(logging);

        //The creation of first child process
        printf("\nCreating the child process now");
        int success = pthread_create(&child1,NULL,child1_funct,(void*)log_file);
        printf("\nThe thread is created with return status %d",success);
		
        int success1 = pthread_create(&child2,NULL,child2_funct,(void*)log_file);
        printf("\nThe thread is created with return status %d",success);
		if(success==0)
            pthread_join(child1,NULL);
		printf("\nOut of child1\n");
		if(kill_msg == 1)
		{
			printf("kill_msg = 1\n");
			fprintf(logging,"\n>>>>>>>>>> [%lld usec] Child 1 exiting due to USR1 signal <<<<<<<<<\n",timestamp1());
		}
		if(kill_msg == 2){
			printf("kill_msg = 2\n");
			fprintf(logging,"\n>>>>>>>>>> [%lld usec] Child 1 exiting due to USR2 signal <<<<<<<<<\n",timestamp1());
		}
        if(success1 == 0)
            pthread_join(child2,NULL);
		printf("\nOut of child2\n");
		if(kill_msg == 1)
		{
			printf("kill_msg = 1\n");
			fprintf(logging,"\n>>>>>>>>>> [%lld usec] Child 2 exiting due to USR1 signal <<<<<<<<<\n",timestamp1());
			fclose(logging);
		}
		if(kill_msg == 2){
			printf("kill_msg = 2\n");
			fprintf(logging,"\n>>>>>>>>>> [%lld usec] Child 2 exiting due to USR2 signal <<<<<<<<<\n",timestamp1());
			fclose(logging);
		}
    }
    return 0;
}


/*
* Function name: child1_fun()
* Description: It opens the file and writes the report of the characters in the file and exits
* @param: void* log_file - name of the log file to log the report.
* @return: static void* - status of the child creation if successful or not.
*/
static void *child1_funct(void* log_file)
{
    FILE *text_file_ptr;
    char *text_file_data;
	long length;
	char temp;
	int count = 1;
	static int first_time = 0;
	
	struct pthreads_sub
	{
    	char* log_file_name;
	};

    /* Logging the  */
    struct pthreads_sub *my_log_file = (struct pthreads_sub *)log_file;
    printf("\nInside the child1 function");
    printf("\nThe name of the log file is %s",my_log_file->log_file_name);
    logging = fopen(my_log_file->log_file_name,"a");
    if(logging == NULL)
        printf("\nCannot open the file");
    else
    {
        //printf("\nFile opened successfully");
        fprintf(logging,"[%lld usec] In the Child1 thread POSIX thread id = %ld, LINUX thread id = %ld\n", timestamp1(), pthread_self(), syscall(SYS_gettid));
        //fclose(logging);
    }

    text_file_ptr = fopen("gdb.txt","r");
    if(text_file_ptr)
    {
        fseek(text_file_ptr, 0, SEEK_END);
        length = ftell(text_file_ptr);
        fseek(text_file_ptr, 0, SEEK_SET);
        text_file_data = (char*)malloc((length+1)*sizeof(char));
        if(text_file_data)
        {
            fread(text_file_data, sizeof(char), length, text_file_ptr);
        }
        fclose(text_file_ptr);
        text_file_data[length] = '\0';
    }

	for(int i=0; i<length-1; i++)
	{
		for(int j=i+1; j<length; j++)
		{
			if(text_file_data[i] >=65 && text_file_data[i] <=90)
			{
				text_file_data[i] += 32; 							//converting to lower case
			}
			if(text_file_data[j] >=65 && text_file_data[j] <=90)
			{
				text_file_data[j] += 32; 							//converting to lower case
			}
			if(strcmp((text_file_data+i),(text_file_data+j))>0)
			{
				temp = text_file_data[i];
				text_file_data[i] = text_file_data[j];
				text_file_data[j] = temp;
			}
		}
	}

	report = (struct reporting*)malloc(sizeof(struct reporting));
	for(int i=0; i<length; i++)
	{
		if(text_file_data[i] >= 97 && text_file_data[i] <= 122)
		{
			if(*(text_file_data+i)==*(text_file_data+i+1))
			{
				count = count + 1;
			}
			else
			{
				if(first_time == 0)
				{
					if(count < 100)
					{
						add_new_member((text_file_data+i),count);
						head = report;
						first_time = 1;
					}
				}
				else
				{
					if(count < 100)
						add_new_member((text_file_data+i),count);
				}
			count = 1;				
			}
		}
	}
	print_list();
	fprintf(logging,"[%lld usec] >>>>>>>>>> Child 1 Exiting normally <<<<<<<<<<\n", timestamp1());
    fclose(logging);
    free(text_file_data);
    pthread_exit(NULL);
}

void add_new_member(char* member,int count)
{
	struct reporting *ptr = (struct reporting*)malloc(sizeof(struct reporting));
	report->next = ptr;
	ptr->alphabet = *member;
	ptr->count = count;
	ptr->next = NULL;
	report = ptr;
}

void print_list()
{
	struct reporting *ptr;
	ptr = head;
	while(ptr->next != NULL)
	{
		fprintf(logging,"\n[%lld usec] Character - %c has count %d", timestamp1(), ptr->alphabet,ptr->count);
		ptr = ptr->next;
	}
	fprintf(logging,"\n[%lld usec] Character - %c has count %d", timestamp1(), ptr->alphabet,ptr->count);
}

static void *child2_funct(void* log_file)
{
    FILE *text_file_ptr;
    char *text_file_data;
    struct itimerspec timer_config;
    struct sigevent sigev;

	struct pthreads_sub
	{
    	char* log_file_name;
	};

	FILE *text_file_ptr1;
   	char text_file_data1[10000];
	long length=10000;

    struct pthreads_sub *my_log_file = (struct pthreads_sub *)log_file;
    printf("\nInside the child2 function");
    printf("\nThe name of the log file is %s",my_log_file->log_file_name);

    logging = fopen(my_log_file->log_file_name,"a");
    if(logging == NULL)
        printf("\nCannot open the file");
    else
    {
        printf("\nFile opened successfully");
        fprintf(logging,"\n[%lld usec] In the Child2 thread POSIX thread id = %ld, LINUX thread id = %ld\n", timestamp1(), pthread_self(),syscall(SYS_gettid));
    }

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo  = SIGRTMIN;
    sigev.sigev_value.sival_ptr = &timerid;

    timer_create(CLOCK_REALTIME, &sigev, &timerid);

    timer_config.it_interval.tv_nsec = 100000000;
    timer_config.it_interval.tv_sec  = 0;

    timer_config.it_value.tv_nsec    = 100000000;
    timer_config.it_value.tv_sec     = 0;

    int success = timer_settime(timerid, 0, &timer_config, NULL);
    free(text_file_data);
	while(1)
	{
	if(print_stat == 1)
	{
		text_file_ptr1 = fopen("/proc/stat","r");
    	if(text_file_ptr1)
    	{
        	{
        	    fread(text_file_data1, sizeof(char), length, text_file_ptr1);
        	}
        	fclose(text_file_ptr1);
        	text_file_data1[length] = '\0';
    	}
		else
			printf("File not opened");    	
		logging = fopen(my_log_file->log_file_name,"a");
		fprintf(logging,"\n[%lld usec] %s",timestamp1(),text_file_data1);
		print_stat = 0;
	}
	}
    //pthread_exit(NULL);
}

// credits: https://www.epochconverter.com/programming/c. Below function is referenced from websitef
void timestamp()
{
	time_t time_now;
	struct tm ts;
	char time1[80];
	
	time(&time_now);
	
	ts = *localtime(&time_now);
	strftime(time1, sizeof(time1), "[%a %Y-%m-%d %H:%M:%S %Z]", &ts);
	printf("%s",time1);
}

// credits: https://programmingtoddler.wordpress.com/2014/11/09/c-how-to-get-system-timestamp-in-second-millisecond-and-microsecond/  Below function is referenced from website
long long timestamp1()
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
    switch(sig)
    {
    case 34:
		print_stat = 1;
		printf("Timer interrupt\n");
        break;
    case SIGUSR1:
		kill_msg = 1;
		printf("Inside USR1 signal");
		pthread_exit(NULL);
		break;
    case SIGUSR2:
		kill_msg = 2;
		printf("Inside USR2 signal");
		pthread_exit(NULL);
		break;
    }
}
