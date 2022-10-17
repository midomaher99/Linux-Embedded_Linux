/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File: shelldemmo.c
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "prompt.h"
#include "loc_vars.h"
#include "commands.h"
#include "interpreter.h"

/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/
int save_to_history(int history_fd,char* input_line, ssize_t  input_length);

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/
int main ()
{
	int ret_val=1;
	command_t command = {NULL};
	int prompt_status ;
	char* input_line;	//the input from user
	ssize_t  input_length;
	char* username =NULL;
	char history_file_path [70]={"/home/"};
	int history_fd=-1;

	//history file
	if((username=getenv("USER")) ==NULL)
	{
		printf("Error getting the username to open the history file.\n");
		printf("shellDemmo will executed without commands history saving/n.");

	}
	else
	{
		strcat(history_file_path,username);
		strcat(history_file_path,"/.mybash_history");
		history_fd= open (history_file_path, O_CREAT |O_APPEND |O_RDWR,0666);	//for commands history save
		//check open return
		if (history_fd == -1)
		{
			printf("Error openning /home/maher/.mybash_history to save commands history.\n");
			printf("shellDemmo will be executed without commands history saving/n.");
			printf("errno = %d\n",errno);
		}
	}

	// temp files for piping
	if ((pipe_tmp_file1_fd =mkstemp(pipe_tmp_file1_path)) == -1)
	{
		printf("Error creating temp files for piping.\n");
		printf("errno= %d",errno);
		return -15;
	}
	else
	{
		if((unlink(pipe_tmp_file1_path))==-1)
		{
			printf("Error unlinking temp files for piping.\n");
			printf("errno= %d",errno);
			return -16;
		}
	}

	if ((pipe_tmp_file2_fd =mkstemp(pipe_tmp_file2_path)) == -1)
		{
			printf("Error creating temp files for piping.\n");
			printf("errno= %d",errno);
			return -15;
		}
		else
		{
			if((unlink(pipe_tmp_file2_path))==-1)
			{
				printf("Error unlinking temp files for piping.\n");
				printf("errno= %d",errno);
				return -16;
			}
		}

/*__________________________________________________________________________*/
	while(1)
	{
		//re-init the affected values
		command.ptr_args_arr=NULL;
		input_line=NULL;

		//printing prompt message
		prompt_status=print_prompt();

		// check the return value of print_prompt
		switch (prompt_status)
		{
		case 0 :
			break;

		case -1:
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

		input_length ++;//the extra space will be used in the interpreter
		input_line=realloc(input_line,(size_t) input_length);//avoid over-sizing done by getline()
		if(input_line == NULL)
		{
			ret_val =-4;
			printf("Error allocating the input line buffer.\n");
			break;
		}
		//allocating the args array with the length of the input line for worst case
		command.ptr_args_arr =(char*(*)[]) malloc(input_length*sizeof(char*));
		if(command.ptr_args_arr == NULL)
		{
			ret_val=-5;
			printf("Error allocating arguments buffer.\n");
			break;
		}
		//parse and execute the line commands

		interpreter(&command, input_line);

		//free dynamic allocated memory
		free(input_line);
		free(command.ptr_args_arr);
	}

	return ret_val;
}


/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/
/******************************************************************************
 * \Syntax          : int save_to_history(int history_fd,char* input_line, ssize_t  input_length)
 * \Description     : save the input line to the commands history

 * \Parameters (in) : int history_fd : the file descriptor of the file where commands history saved
 *					  char* input_line : the input line to be saved
 * \Return value:   : int 1 : if success
 * 						 -1 : if writing to the history file failed
 *******************************************************************************/
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
/**********************************************************************************************************************
 *  END OF FILE: shellDemmo.c
 *********************************************************************************************************************/
