/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  arguments.c
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "errno.h"
#include <stdio.h>
#include "commands.h"
#include "loc_vars.h"
/**********************************************************************************************************************
 *  LOCAL MACROS CONSTANT\FUNCTION
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/
typedef enum
{
	not_exist,
	exist
}binaryFlag_t;
typedef enum
{
	no_redir,
	input,
	output
}redirectFlag_t;

/**********************************************************************************************************************
 *  LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/





int external_redirection(command_t* command)
{
	int fd;
	int ret_val =1;
	if(command->stdfiles.in != NULL)
	{
		if((fd= open(command->stdfiles.in, O_CREAT | O_RDWR,0666)) == -1)
		{
			printf("Error opening %s to redirect output.\n", command->stdfiles.in);
			printf("errno = %d\n",errno);
			ret_val=-1;
		}
		if(dup2(fd,0) == -1)
		{
			printf("Error duplicating %s to redirect output.\n", command->stdfiles.in);
			printf("errno = %d\n",errno);
			ret_val=-2;
		}
		if(close(fd) == -1)
		{
			printf("Error closing %s .\n", command->stdfiles.in);
			printf("errno = %d\n",errno);
			ret_val =-3;
		}
	}
	if(command->stdfiles.out != NULL)
	{
		if((fd= open(command->stdfiles.out, O_CREAT | O_RDWR,0666)) == -1)
		{
			printf("Error opening %s to redirect output.\n", command->stdfiles.out);
			printf("errno = %d\n",errno);
			ret_val=-4;
		}
		if(dup2(fd,1) == -1)
		{
			printf("Error duplicating %s to redirect output.\n", command->stdfiles.out);
			printf("errno = %d\n",errno);
			ret_val=-5;
		}
		if(close(fd) == -1)
		{
			printf("Error closing %s .\n", command->stdfiles.out);
			printf("errno = %d\n",errno);
			ret_val =-6;
		}
	}

	if(command->stdfiles.err != NULL)
	{
		if((fd= open(command->stdfiles.err, O_CREAT | O_RDWR,0666)) == -1)
		{
			printf("Error opening %s to redirect error.\n", command->stdfiles.err);
			printf("errno = %d\n",errno);
			ret_val=-7;
		}
		if(dup2(fd,2) == -1)
		{
			printf("Error duplicating %s to redirect error.\n", command->stdfiles.err);
			printf("errno = %d\n",errno);
			ret_val=-8;
		}
		if(close(fd) == -1)
		{
			printf("Error closing %s .\n", command->stdfiles.err);
			printf("errno = %d\n",errno);
			ret_val =-9;
		}
	}
	return ret_val;
}
/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : void extract(char* line, ssize_t length, char** (**args)[]);
 * \Description     : this function extracts the arguments of commands entered by user
 * 					  in a '\n' terminated line.
 * 					  the arguments of each command are sequentially pointed *args and a '\0' separates
 * 					  between the arguments of each command
 * 					  ex:(NOTE: set the tab size to 4 spaces)
 *
 * 					 command no:|------1-----|    |-2-|    |-----------3----------|
 * 					  			 ________________________________________________________________
 * 					 		    | | |    | | |    | | |    | | | |    | | | | | | |    |	|	 |
 * 					 input line:|l|s|    |-|a|  ; |p|s|  ; |v|i|m|    |m|a|i|n|.|c|'\n'|'\0'|'\0'|
 *					   		    |_|_|____|_|_|____|_|_|____|_|_|_|____|_|_|_|_|_|_|____|____|____|
 *
 *		 					  	 ________________________________________________________________
 * 					 		    | | |    | | |    | | |    | | | |    | | | | | | |    |	|	 |
 * 	 input line after executing:|l|s|'\0'|-|a|'\0'|p|s|'\0'|v|i|m|'\0'|m|a|i|n|.|c|'\0'|'\n'|'\0'|
 *					   		    |_|_|____|_|_|____|_|_|____|_|_|_|____|_|_|_|_|_|_|____|____|____|
 *					  			 ^		  ^		   ^		^		   ^				^
 *					  			 |________| _______| _______|__________|________________|_____________
 * 					 		    ||		 ||  |	  ||  |	   ||		  ||		  |	   ||	|	 |	  |
 * 	 					   ARGS:||       ||  |'\0'||  |'\0'||	 	  ||          |'\0'||   |'\0'|'\n'|
 *					   		    |________|___|____|___|____|__________|___________|____|____|____|____|
 *
 *
 * \Parameters (in) : char* line 	   : the input from the user the line is '\n' terminated
 * 					  ssize_t length   : the input line length + 2
 * \Parameters (out): char* (**args)[]: will be dynamically allocated to contain the commands' arguments
 * 										 must be freed after calling the function
 * \Return value:   : None
 *******************************************************************************/


int parser(command_t* command, char* input_line, int* start_index)
{
	binaryFlag_t separator_flag=exist;	//exist when the last char is a separator (space, ';', '\n' or whatever except regular character)
	redirectFlag_t redir_flag = no_redir;	//when redirection exist
	int args_iterator=0;
	int line_iterator;

	//tokenizing the input line
	//parsing only one command in case of multi-command line
	for (line_iterator=(*start_index); (input_line[line_iterator]!=';' && input_line[line_iterator]!='\n');line_iterator++)
	{

		switch (input_line[line_iterator])
		{
		case ' ':
			if(separator_flag == exist)
			{
				//do nothing
			}
			else//flag not exist
			{
				separator_flag=exist;
				input_line[line_iterator]='\0';
			}
			break;

		case '>':
			input_line[line_iterator]='\0';
			redir_flag = output;
			break;

		case '<':
			input_line[line_iterator]='\0';
			redir_flag = input;
			break;

		default:	//no special character except de-referencing variables will be done later
			if(redir_flag == output)
			{
				command->stdfiles.out = &input_line[line_iterator];
			}
			else if(redir_flag ==input)
			{
				command->stdfiles.in=&input_line[line_iterator];
			}
			else if(separator_flag == exist)
			{
				(*(command->ptr_args_arr))[args_iterator]=&input_line[line_iterator];

				args_iterator++;
			}
			else
			{
				//do nothing
			}
			separator_flag = not_exist;
			redir_flag = not_exist;
			break;
		}
	}

	//commands termination
	if(input_line[line_iterator]==';')
	{
		input_line[line_iterator] ='\0';	//terminating last token be '\0'
		line_iterator+=1;
	}
	else if(input_line[line_iterator]=='\n')
	{
		input_line[line_iterator]='\0';		//terminating last token be '\0'
		line_iterator++;
		input_line[line_iterator]='\n';		//add a dummy command at the end of all commands '\n' which will be handles as an enter press
		(*(command->ptr_args_arr))[args_iterator]='\0';//terminate the dummy command

	}

	//variables evaluating
	char* variable=NULL;

	for (int i=0;i<args_iterator;i++)	//loop for all args to find the dereference symbol '$'
	{
		if((*command->ptr_args_arr)[i] !=(char*) '\0' &&((*command->ptr_args_arr)[i])[0] == '$' && strlen((*command->ptr_args_arr)[i]) != 1)	//variable needs to be dereferenced
		{
			if((variable=getenv(((*command->ptr_args_arr)[i])+1)) != NULL)	//environmental variable
				(*command->ptr_args_arr)[i] = variable;

			else if((variable=getloc(((*command->ptr_args_arr)[i])+1)) != NULL)	//local variable
				(*command->ptr_args_arr)[i] = variable;

			else	//not a variable?? then ignore it by shifting all next args back by one step
			{
				for(int ignore_iterator =i;(*command->ptr_args_arr)[ignore_iterator] != (char*)'\0';ignore_iterator++)
				{
					(*command->ptr_args_arr)[ignore_iterator] = (*command->ptr_args_arr)[ignore_iterator+1];
				}
				i--;
			}
		}

	}

	return line_iterator;	//the index of the start of new command
}



int executer(command_t* command)
{
	int ret_val=1;
	int internal_command_index;	//
	int val_index;
	if((internal_command_index=is_internal_command(command))!=-1)
	{
		ret_val=internal_executer(command, internal_command_index);

	}
	else if((val_index=is_variable(command))!=-1)
	{
		ret_val=variable_executer(command, val_index);
	}
	else	//extenal command
	{

		pid_t ret_pid;
		int child_status;
		ret_pid=fork();
		if(ret_pid < 0)//faild
		{
			printf("Error while fork to execute external command\n");
			printf("errno = %d\n",errno);
			ret_val =-1;

		}
		else if(ret_pid > 0)//parent
		{
			if(wait(&child_status) == -1)
			{
				printf("Error while waiting for the child process status\n");
				printf("errno = %d\n",errno);
			}

		}
		else//child
		{
			//redirections
			external_redirection(command);

			//exec
			execvp((*command->ptr_args_arr)[0],*command->ptr_args_arr);
		}
	}
	command->stdfiles.in=NULL;
	command->stdfiles.out=NULL;
	command->stdfiles.err=NULL;
}







int interpreter(command_t* command, char* input_line)
{
	int index_new_command=0;
	//parsing a line
	/*
	 * in case of line contains multi-commands, commands will be parsed
	 * and executed sequentially
	 */

	while(input_line[index_new_command] != '\n')	//loop till the end of line(last command)
	{

		//parser will return every time a single command parsed
		index_new_command=parser(command, input_line, &index_new_command);
		//cheack return
		//execute the parsed command
		executer(command);
		//check return

	}

}

/**********************************************************************************************************************
 *  END OF FILE: arguments.c
 *********************************************************************************************************************/
