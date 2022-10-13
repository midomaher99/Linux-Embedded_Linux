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

#include "arguments.h"

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


int parser(command_t* command, char* line, int* start_index)
{
	binaryFlag_t separator_flag=exist;	//exist when the last char is a separator (space, ';', '\n' or whatever except regular character)
	redirectFlag_t redir_flag = not_exist;	//when redirection exist
	int args_iterator=0;
	int line_iterator;

	//extracting method is shown in a flow chart in README.md file
	for (line_iterator=(*start_index);line[line_iterator]!=';'&&line[line_iterator]!='\n';line_iterator++)	//loop on the input
	{

		switch (line[line_iterator])
		{
		case ' ':
			if(separator_flag == exist)
			{
				//do nothing
			}
			else//flag not exist
			{
				separator_flag=exist;
				line[line_iterator]='\0';
			}
			break;
		case '>':
			line[line_iterator]='\0';
			redir_flag = output;
			break;
		case '<':
			line[line_iterator]='\0';
			redir_flag = input;
			break;
		default:
			if(redir_flag == output)
			{
				command->stdfiles.out = &line[line_iterator];
			}
			else if(redir_flag ==input)
			{
				command->stdfiles.in=&line[line_iterator];
			}
			else if(separator_flag == exist)
			{
				(*(command->args))[args_iterator]=&line[line_iterator];
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

	if(line[line_iterator]==';')
	{
		line[line_iterator] ='\0';
		line_iterator+=1;
	}
	else if(line[line_iterator]=='\n')
	{line[line_iterator]='\0';
	line_iterator++;
	line[line_iterator]='\n';
	(*(command->args))[args_iterator]='\0';
	}
	return line_iterator;
}





void executer(command_t* command)
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
		execvp((*command->args)[0],*command->args);
	}
}










/**********************************************************************************************************************
 *  END OF FILE: arguments.c
 *********************************************************************************************************************/
