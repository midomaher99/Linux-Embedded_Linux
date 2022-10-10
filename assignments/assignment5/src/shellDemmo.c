#include "prompt.h"
#include "stdio.h"
#include "arguments.h"


#include "stdlib.h"

#define ARGS_ARRAY 	   (*args)
#define PTR_ARGS_ARRAY (args)

int main ()
{
//printing prompt message
	int status;
	status=print_prompt();
	
// chech the return value of print_prompt
	switch (status)
	{
		case 0 :
			break;

		case USER_NO_MATCH:
			printf("Error getting the username.\n");
			printf("No match for \"USERNAME\" environment variable.\n");
			return -1;
			break;

		default:
		
			printf("Error getting the path of the current working directory.\n");
			printf("errno = %d\n",status);
			return -1;
			break;
	}

//get input
	char* line=NULL;
	ssize_t  length=0;
	length=getline(&line,(size_t*) &length,stdin);

	length+=2;	// one to add '\0' at the end of the line in extract function and the other one will be used in special case in extract

	line=realloc(line,(size_t) length+1);//avoid oversizing in getline()

//extracting commands arguments from input 
	char **(*args)[]= NULL;
	extract(line, length, &PTR_ARGS_ARRAY);

//executer
	int i;

	for (i=0;((*args)[i]!='\n');i++)
		printf("%p\n", ARGS_ARRAY[i]);

//free dynamic allocated memory
	free(args);
	free(line);


	return 0;
}
