#include "prompt.h"
#include "stdio.h"
#include "arguments.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "stdlib.h"

#define ARGS_ARRAY 	   (*args)
#define PTR_ARGS_ARRAY (args)
typedef struct
{
	char* in;
	char* out;
	char* err;
}stdFiles_t;

typedef struct
{
	stdFiles_t stdfiles;	//redirections
	char* (*args)[];		//arguments
}command_t;


int parser(command_t* command, char* line, int* start_index);
void executer(command_t* command);

int main ()
{
	command_t command = {NULL};
	int prompt_status =0;
	char* line=NULL;
	ssize_t  length=0;
	int new=0;
	int history_fd= open ("/tmp/.mybash_history", O_CREAT |O_APPEND |O_RDWR,0666);
	while(1)
	{
		command.args=NULL;
		line=NULL;
		new =0;
		//printing prompt message
		prompt_status=print_prompt();
		// check the return value of print_prompt
		switch (prompt_status)
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
			printf("errno = %d\n",prompt_status);
			return -1;
			break;
		}
		/*___________________________________________________________________________________________*/
		//get input

		length=getline(&line,(size_t*) &length,stdin);

		if (length != 1 && line[0] != '\n')
			write(history_fd, line,length);

		length ++;
		line=realloc(line,(size_t) length);//avoid oversizing in getline()

		command.args =(char*(*)[]) malloc(length*sizeof(char*));
		/*___________________________________________________________________________________________*/
		//parse a command
		while(line[new] != '\n')
		{

			new=parser(&command, line, &new);
			//printf("%s",(*command.args)[0]);
			executer(&command);



			command.stdfiles.in=NULL;
			command.stdfiles.out=NULL;
		}







		//free dynamic allocated memory
		free(line);
		free(command.args);
	}
	return 0;
}
