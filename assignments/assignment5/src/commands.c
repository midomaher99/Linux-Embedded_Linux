/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  commands.c
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/


#include "commands.h"
#include "config.h"
#include "errno.h"
#include "loc_vars.h"
#include "stdio.h"
#include "internal_commands.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "unistd.h"
#include "string.h"
/**********************************************************************************************************************
 *  LOCAL MACROS CONSTANT\FUNCTION
 *********************************************************************************************************************/

#define NUM_OF_INTERNAL_COMMANDS	(5u)
#define HISTORY_READ_BUFF_SIZE	(256u)
/**********************************************************************************************************************
 *  LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/
internalcommand_t internal_commands_array[NUM_OF_INTERNAL_COMMANDS]=
{
		{"myset",&myset},
		{"myexport",&myexport},
		{"myhistory",&myhistory},
		{"mycd",&mycd},
		{"mypwd",&mypwd}

};


/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : int is_internal_command(command_t* command)
 * \Description     : check if the given command is an internal or not
 *

 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  command's args

 * \Return value:   : int -1 : if not an internal command
 *
 * 					command index in internal_commands_array:  if an internal command
 *******************************************************************************/

int is_internal_command(command_t* command)
{
	int ret_val =-1;
	for(int command_index=0;command_index<NUM_OF_INTERNAL_COMMANDS;command_index++)
	{
		if(strcmp((*command->ptr_args_arr)[0],internal_commands_array[command_index].command_name) == 0)
			ret_val= command_index;
	}
	return ret_val;
}



/******************************************************************************
 * \Syntax          : int internal_executer(command_t*  command,int internal_command_index)
 * \Description     : executes the given internal command
 *

 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  command's args
 * 					  int internal_command_index: the command index in internal_commands_array

 * \Return value:   : int 1
 *******************************************************************************/

int internal_executer(command_t*  command,int internal_command_index)
{
	int ret_val =1;
	int argc=0;
	for(int i =0;(*command->ptr_args_arr)[i]!=(char*)'\0' ;i++)
		argc++;


	ret_val=internal_commands_array[internal_command_index].ptr_command(argc,&((*command->ptr_args_arr)[0]));

	return ret_val;
}
/**********************************************************************************************************************
 *  INTERNAL COMMANDS
 *********************************************************************************************************************/
int mycd(int argc, char*argv[])
{
	int ret_val =1;
	if(argc>2)
	{
		printf("too much arguments\n");
		ret_val = -1;
	}
	else if(argc<2)
	{
		printf("not enough arguments arguments\n");
		ret_val = -2;
	}
	else{
		if(chdir(argv[1]) == -1)
		{
			printf("Error changing directory.\n");
			printf("errno = %d\n", errno);
			ret_val =-3;
		}
	}
	return ret_val;
}

int myhistory(int argc, char*argv[])
{
	int ret_val=1;

	if(argc>1)
	{
		printf("too much arguments\n");
		ret_val = -1;
	}
	else
	{
		ssize_t read_size;
		ssize_t write_size;
		char read_buff[HISTORY_READ_BUFF_SIZE];

		int history_fd= open ("/tmp/.mybash_history.log", O_RDONLY);

		if(history_fd == -1)
		{
			printf("Error  /tmp/.mybash_history.log to read commands history.\n");
			printf("errno = %d\n",errno);
			ret_val= -2;
		}
		while ((read_size = read(history_fd,read_buff,HISTORY_READ_BUFF_SIZE))!=0) //not end of the file
		{
			if(read_size == -1)
			{
				printf("Error reading /tmp/.mybash_history.log\n");
				printf("errno = %d\n",errno);
				ret_val= -3;
			}
			else
			{
				if((write_size=write(1,read_buff,read_size))==-1)
				{
					printf("Error printing commands history to stdout.\n");
					printf("errno = %d\n",errno);
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
	int ret_val=1;
	if(argc>1)
	{
		printf("too much arguments\n");
		ret_val = -1;
	}
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
	{
		printf("too much arguments\n");
		ret_val = -1;
	}
	else if(argc<2)
	{
		printf("not enough arguments arguments\n");
		ret_val = -2;
	}
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
		if (ret_val !=1)
			printf("variable \"%s\" not found in local variables\n", argv[1]);

	}
	return ret_val;
}

int mypwd(int argc, char*argv[])
{
	int ret_val=1;
	char* path;
	if(argc>1)
	{
		printf("too much arguments\n");
		ret_val = -1;
	}
	if((path=getcwd(NULL, 0) )==NULL)
	{
		printf("Error getting current working directory.\n");
		printf("errno = %d", errno);
		ret_val =-2;
	}
	else
	{
		printf("%s\n",path);
	}
	free(path);
	return ret_val;
}
/**********************************************************************************************************************
 *  END OF FILE: commands.c
 *********************************************************************************************************************/
