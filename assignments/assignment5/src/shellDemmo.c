#include "prompt.h"
#include "stdio.h"
#include "arguments.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "stdlib.h"

#define ARGS_ARRAY
#define PTR_ARGS_ARRAY
#define NUMBER_OF_VARIABLES	256
typedef struct
{
	char* var;
	char* val;
}locVariable_t;
typedef struct
{
	char* in;
	char* out;
	char* err;
}stdFiles_t;

typedef struct
{
	stdFiles_t stdfiles;	//redirections
	char* (*ptr_args_arr)[];		//arguments
}command_t;


int parser(command_t* command, char* input_line, int* start_index);
void executer(command_t* command);
locVariable_t* loc_vars_array[NUMBER_OF_VARIABLES];
int variables_counter=0;

int save_to_history(int history_fd,char* input_line, ssize_t  input_length)
{
	int ret_val=1;
	ssize_t write_size;
	if (input_length != 1 && input_line[0] != '\n') //not only enter
	{
		if((write_size=write(history_fd, input_line,input_length))== -1)
		{
			ret_val=-1;
		}
	}
	return ret_val;
}

int main ()
{
	int ret_val=1;

	command_t command = {NULL};
	int prompt_status ;
	char* input_line;	//the input from user
	ssize_t  input_length;
	int index_new_command;	//the index of the first char of the new command in case of lines of multi-commands

	//local variables array initialization to make its initial value compiler independent
	for(int i =0 ;i< NUMBER_OF_VARIABLES;i++)
		loc_vars_array[i]=NULL;

	int history_fd= open ("/tmp/.mybash_history", O_CREAT |O_APPEND |O_RDWR,0666);	//for commands history save

	//check open return
	if (history_fd == -1)
	{
		printf("Error openning /tmp/.mybash_history to save commands history.\n");
		printf("shellDemmo will executed without commands history saving/n");
		printf("errno = %d\n",errno);
	}

	while(1)
	{
		//re-init the affected values
		command.ptr_args_arr=NULL;
		input_line=NULL;
		index_new_command =0;

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
			ret_val = -1;
			break;

		default:

			printf("Error getting the path of the current working directory.\n");
			printf("errno = %d\n",prompt_status);
			ret_val = -2;
			break;
		}
		if(ret_val != 1)	//failure in printing prompt message
			break;

		//get input
		if((input_length=getline(&input_line,(size_t*) &input_length,stdin)) == -1)
		{
			printf("Error getting the input from the user.\n");
			printf("errno = %d\n",errno);
			ret_val = -3;
			break;
		}

		//save input line to history
		if(save_to_history(history_fd,input_line, input_length)==-1)
		{
			printf("Error saving this line to commands history\n");
			printf("This line will be executed but will not be saved to commands history\n");
			printf("errno = %d\n",errno);
		}

		input_length ++;//the extra space will be used in the parser
		input_line=realloc(input_line,(size_t) input_length);//avoid over-sizing done by getline()

		//allocating the args array with the length of the input line for worst case
		command.ptr_args_arr =(char*(*)[]) malloc(input_length*sizeof(char*));

		//parsing a line
		/*
		 * in case of line contains multi-commands, commands will be parsed
		 * and executed sequentially
		 */
		while(input_line[index_new_command] != '\n')	//loop till the end of line(last command)
		{

			//parser will return every time a single command parsed
			index_new_command=parser(&command, input_line, &index_new_command);
			//cheack return
			//execute the parsed command
			executer(&command);
			//check return
			//re-init the std-files to avoid unnecessary redirection
			command.stdfiles.in=NULL;
			command.stdfiles.out=NULL;
		}

		//free dynamic allocated memory
		free(input_line);
		free(command.ptr_args_arr);
	}

	return ret_val;
}
