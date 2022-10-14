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

#include "arguments.h"







#include <stdio.h>

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

#define NUM_OF_INTERNAL_COMMANDS	(3u)

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

typedef struct
{
	char* var;
	char* val;
}locVariable_t;
extern locVariable_t* loc_vars_array[256];
char *getloc(const char *name)
{
	char* ret_val=NULL;
	for(int i =0; loc_vars_array[i] != NULL; i++)
	{
		if ((strcmp((loc_vars_array[i]->var), name)) == 0)
		{
			ret_val=loc_vars_array[i]->val;
			break;
		}
	}
	return ret_val;
}
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
		if((*command->ptr_args_arr)[i] != '\0' &&((*command->ptr_args_arr)[i])[0] == '$' && strlen((*command->ptr_args_arr)[i]) != 1)	//variable needs to be dereferenced
		{
			if((variable=getenv(((*command->ptr_args_arr)[i])+1)) != NULL)	//environmental variable
				(*command->ptr_args_arr)[i] = variable;

			else if((variable=getloc(((*command->ptr_args_arr)[i])+1)) != NULL)	//local variable
				(*command->ptr_args_arr)[i] = variable;

			else	//not a variable?? then ignore it by shifting all next args back by one step
			{
				for(int ignore_iterator =i;(*command->ptr_args_arr)[ignore_iterator] != '\0';ignore_iterator++)
				{
					(*command->ptr_args_arr)[ignore_iterator] = (*command->ptr_args_arr)[ignore_iterator+1];
				}
			}
		}

	}

	return line_iterator;
}

typedef struct
{
	char* command_name;
	int(*ptr_command)(int argc, char*argv[]);
}internalcommand_t;
#define HISTORY_READ_BUFF_SIZE	(200u)
int myhistory(int argc, char*argv[])
{
	int ret_val=1;
	if(argc>1)
		ret_val = -1;
	else
	{
		int history_fd= open ("/tmp/.mybash_history", O_RDONLY);
		ssize_t read_size;
		ssize_t write_size;
		char read_buff[HISTORY_READ_BUFF_SIZE];
		if(history_fd == -1)
			ret_val= -2;
		while ((read_size = read(history_fd,read_buff,HISTORY_READ_BUFF_SIZE))!=0) //not end of the file
		{
			if(read_size == -1)
				ret_val= -3;
			else
			{
				if((write_size=write(1,read_buff,read_size))==-1)
				{
					ret_val=-4;
					break;
				}
			}
		}
	}
	return ret_val;
}
int myset(int argc, char*argv[])
{
	int ret_val=0;
	if(argc>1)
		ret_val = -1;
	else
	{
		for(int i=0;loc_vars_array[i]!=NULL;i++)
		{
			printf("%s=%s\n",loc_vars_array[i]->var,loc_vars_array[i]->val);
		}
	}
	return ret_val;
}
int myexport(int argc, char*argv[])
{
	int ret_val=0;
	if(argc>2)
		ret_val = -1;
	else if(argc<2)
		ret_val = -2;
	else
	{
		for(int i=0;loc_vars_array[i]!=NULL;i++)
		{
			if(strcmp(loc_vars_array[i]->var,argv[1]) ==0)
			{
				ret_val = 1;
				setenv(loc_vars_array[i]->var,loc_vars_array[i]->val,1);
				break;
			}
		}

	}
	return ret_val;
}
internalcommand_t internal_commands_array[NUM_OF_INTERNAL_COMMANDS]=
{
		{"myset",&myset},
		{"myexport",&myexport},
		{"myhistory",&myhistory}
};


int is_internal_command(command_t* command)
{
	for(int command_index=0;command_index<NUM_OF_INTERNAL_COMMANDS;command_index++)
	{
		if(strcmp((*command->ptr_args_arr)[0],internal_commands_array[command_index].command_name) == 0)
			return command_index;
	}
	return -1;
}
int is_variable(command_t* command)
{
	int ret_val=-1;
	for(int i =0;((*command->ptr_args_arr)[0])[i] != '\0';i++)
	{
		if(((*command->ptr_args_arr)[0])[i] == '=')
		{
			ret_val=i+1;
			((*command->ptr_args_arr)[0])[i] = '\0';
			break;
		}
	}
	return ret_val;
}
extern int variables_counter;
void executer(command_t* command)
{
	int command_index;
	int val_index;
	if((command_index=is_internal_command(command))!=-1)
	{
		int argc=0;

		for(int i =0;(*command->ptr_args_arr)[i]!='\0' ;i++)
			argc++;

		internal_commands_array[command_index].ptr_command(argc,&((*command->ptr_args_arr)[0]));
	}
	else if((val_index=is_variable(command))!=-1)
	{
		if (variables_counter>=NUM_OF_INTERNAL_COMMANDS)
		{
			//can't add more variables
		}
		else
		{
			loc_vars_array[variables_counter]=(locVariable_t*)malloc(sizeof(locVariable_t));
			loc_vars_array[variables_counter]->var=(char*)malloc(strlen((*command->ptr_args_arr)[0])*sizeof (char));
			strcpy(loc_vars_array[variables_counter]->var, (*command->ptr_args_arr)[0]);
			loc_vars_array[variables_counter]->val=(char*)malloc(strlen(((*command->ptr_args_arr)[0])+val_index)*sizeof (char));
			strcpy(loc_vars_array[variables_counter]->val, ((*command->ptr_args_arr)[0])+val_index);
			variables_counter++;
		}
	}
	else	//extenal command
	{
		pid_t ret_pid;
		int child_status;
		ret_pid=fork();
		if(ret_pid < 0)//faild
		{

		}
		else if(ret_pid > 0)//parent
		{
			wait(&child_status);
		}
		else//child
		{
			int fd;
			//redirections
			if(command->stdfiles.in != NULL)
			{
				fd= open(command->stdfiles.in, O_CREAT | O_RDWR,0666);
				dup2(fd,0);
				close(fd);
			}
			if(command->stdfiles.out != NULL)
			{
				fd= open(command->stdfiles.out, O_CREAT | O_RDWR,0666);
				dup2(fd,1);
				close(fd);
			}
			if(command->stdfiles.err != NULL)
			{
				fd= open(command->stdfiles.err, O_CREAT | O_RDWR,0666);
				dup2(fd,2);
				close(fd);
			}
			//exec
			execvp((*command->ptr_args_arr)[0],*command->ptr_args_arr);
		}
	}
}








/**********************************************************************************************************************
 *  END OF FILE: arguments.c
 *********************************************************************************************************************/
