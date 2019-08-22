#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void main(void)
{

	char a = 'n';
	char b;
	char *input_string, read_string[20];
/* Creating a file*/
	FILE *filept = fopen("test.txt","w");
	
/* Print to standard out*/
	printf("Its a sunny day outside\n");
	if(filept == NULL)
	{
		printf("Allocation failed\n");		
	}
	else
	{
		printf("File created\n");
		fclose(filept);
	}
/* changing permissions, Opening a file, writing a character and closing it */
	chmod ("test.txt", 0600);	
	filept = fopen("test.txt","w");
	fputc(a, filept);
	fclose(filept);

/* Open file in append mode, dynamically allocating array, taking string input from user
writing it to file and losing it */
	filept = fopen("test.txt","a");
	input_string = (char *)malloc(20);
	printf("Input a string to add to file:");
	scanf("%s",input_string);
	fprintf(filept, "\n%s",input_string);
	fflush(filept);
	fclose(filept);

/* Open a file for reading, read a character, read a string of characters, close the file free the memory */
	filept = fopen("test.txt","r");
	b = getc(filept);
	printf("\nCharacter is: %c\n", b);
	fgets(read_string, 20, filept);
	fscanf(filept,"%[^\n]", read_string);
	//puts(read_string);
	printf("String is: %s\n", read_string);
	fclose(filept);
	free(input_string);
}
