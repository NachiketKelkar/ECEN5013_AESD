#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(0));	
	int32_t size = rand() % 4096;
	int32_t sorted[size];
	int32_t to_sort[size];
	int i,s,cases = 1,wrong_size = -6;
	int32_t *null_pointer = NULL;
	time_t current_time = time(NULL);
	struct tm *tm = localtime(&current_time);
	printf("\n\nPID is %d\n",syscall(20));
	printf("Userid is %d\n",syscall(24));
	printf("Current itme and date is: %s\n",asctime(tm));
	printf("Size of the buffer (no of elements) is %d\n",size);
	for(i=0;i<size;i++)
	{
		to_sort[i] = rand() % 16384;
		printf("unsorted[%d] = %d\n",i,to_sort[i]);	
	}
	while(cases != 4){
	switch(cases)
	{
	case 1: 
		printf("Passing correct values\n");
		s = syscall(398, to_sort, sorted, size);
		perror("Invalid arguments to the call\n");
		for(i=0;i<size;i++)
		{
			printf("sorted_array[%d] = %d\n",i,sorted[i]);	
		}
		cases = 2;
		break;
	case 2:
		printf("Passing null pointer\n");
		s = syscall(398, null_pointer, null_pointer, size);
		perror("Invalid arguments to the call\n");
		for(i=0;i<size;i++)
		{
			printf("sorted_array[%d] = %d\n",i,sorted[i]);	
		}
		cases = 3;
		break;
	case 3:
		printf("Passing negative size\n");
		s = syscall(398, to_sort, sorted, wrong_size);
		perror("Invalid arguments to the call\n");
		for(i=0;i<size;i++)
		{
			printf("sorted_array[%d] = %d\n",i,sorted[i]);	
		}
		cases = 4;
		break;
	}}
return 0;
		
}
