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

#define NUM_OF_INTERNAL_COMMANDS	(6u)
#define HISTORY_READ_BUFF_SIZE	(256u)

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/
internalcommand_t internal_commands_array[NUM_OF_INTERNAL_COMMANDS]=
{
		{"myset",&myset},
		{"myexport",&myexport},
		{"myhistory",&myhistory},
		{"mycd",&mycd},
		{"mypwd",&mypwd},
		{"myexit", &myexit}

};

extern char history_file_path [];

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

	int dest_fd;

	int stdin_fd_backup=0;
	int stdout_fd_backup=1;
	int stderr_fd_backup=2;

	//redirect
	if(command->stdfiles.in != NULL)
	{
		stdin_fd_backup=dup(0);

		if((dest_fd= open(command->stdfiles.in, O_CREAT | O_RDWR,0666)) == -1)
		{
			printf("Error opening %s to redirect input.\n", command->stdfiles.in);
			printf("errno = %d\n",errno);
			ret_val=-1;
		}
		if(dup2(dest_fd,0) == -1)
		{
			printf("Error duplicating %s to redirect input.\n", command->stdfiles.in);
			printf("errno = %d\n",errno);
			ret_val=-2;
		}
		if(close(dest_fd) == -1)
		{
			printf("Error closing %s .\n", command->stdfiles.in);
			printf("errno = %d\n",errno);
			ret_val =-3;
		}
	}
	if(command->stdfiles.out != NULL)
	{
		stdout_fd_backup=dup(1);


		if((dest_fd= open(command->stdfiles.out, O_CREAT | O_RDWR,0666)) == -1)
		{
			printf("Error opening %s to redirect output.\n", command->stdfiles.out);
			printf("errno = %d\n",errno);
			ret_val=-4;
		}
		if(dup2(dest_fd,1) == -1)
		{
			printf("Error duplicating %s to redirect output.\n", command->stdfiles.out);
			printf("errno = %d\n",errno);
			ret_val=-5;
		}
		if(close(dest_fd) == -1)
		{
			printf("Error closing %s .\n", command->stdfiles.out);
			printf("errno = %d\n",errno);
			ret_val =-6;
		}
	}

	if(command->stdfiles.err != NULL)
	{

		stderr_fd_backup=dup(2);


		if((dest_fd= open(command->stdfiles.err, O_CREAT | O_RDWR,0666)) == -1)
		{
			printf("Error opening %s to redirect err.\n", command->stdfiles.err);
			printf("errno = %d\n",errno);
			ret_val=-7;
		}
		if(dup2(dest_fd,2) == -1)
		{
			printf("Error duplicating %s to redirect err.\n", command->stdfiles.err);
			printf("errno = %d\n",errno);
			ret_val=-8;
		}
		if(close(dest_fd) == -1)
		{
			printf("Error closing %s .\n", command->stdfiles.err);
			printf("errno = %d\n",errno);
			ret_val =-9;
		}
	}

	//call the internal command pasing argc and argv
	ret_val=internal_commands_array[internal_command_index].ptr_command(argc,&((*command->ptr_args_arr)[0]));


	//restore the default stdfiles
	if(command->stdfiles.in != NULL)
	{
		dup2(stdin_fd_backup, 0);
		close (stdin_fd_backup);
	}
	if(command->stdfiles.out != NULL)
	{
		dup2(stdout_fd_backup, 1);
		close (stdout_fd_backup);

	}
	if(command->stdfiles.err != NULL)
	{
		dup2(stderr_fd_backup, 2);
		close (stderr_fd_backup);
	}

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
	else if(argc==1)
	{
		char*username=NULL;
		char user_path[70]= {"/home/"};
		if((username=getenv("USER")) ==NULL)
		{
			printf("Error getting the username to change directory for /home/USER.\n");
		}
		else
		{
			strcat(user_path, username);

			if(chdir(user_path) == -1)
			{
				printf("Error changing directory.\n");
				printf("errno = %d\n", errno);
				ret_val =-3;
			}
		}
	}
	else
	{
		if(chdir(argv[1]) == -1)
		{
			printf("Error changing directory.\n");
			printf("errno = %d\n", errno);
			ret_val =-4;
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

		int history_fd= open (history_file_path, O_RDONLY);

		if(history_fd == -1)
		{
			printf("Error openning %s to read commands history.\n",history_file_path);
			printf("errno = %d\n",errno);
			ret_val= -2;
		}
		while ((read_size = read(history_fd,read_buff,HISTORY_READ_BUFF_SIZE))!=0) //not end of the file
		{
			if(read_size == -1)
			{
				printf("Error reading %s\n",history_file_path);
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
		printf("\n");
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

int myexit(int argc, char*argv[]) //argc and argv just for convention but not used
{
	_exit(15);	//unique return to know iam terminated from exit

	return 1; //wont return but just for convention of internal commands  int command(int argc, char*argv[])
}
/**********************************************************************************************************************
 *  END OF FILE: commands.c
 *********************************************************************************************************************/
