/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  interpreter.c
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
 *  GLOBAL DATA
 *********************************************************************************************************************/

char pipe_tmp_file1_path[]={"/tmp/piping1-XXXXXX"};
char pipe_tmp_file2_path[]={"/tmp/piping2-XXXXXX"};
int pipe_tmp_file1_fd;
int pipe_tmp_file2_fd;
/**********************************************************************************************************************
 *  LOCAL DATA
 *********************************************************************************************************************/
static binaryFlag_t pipe_flag = not_exist;
static int pipe_counter=0; //for multi piping in same line



/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/


/******************************************************************************
 * \Syntax          : int external_redirection(command_t* command)
 * \Description     : redirect stdfiles for external commands if redirection is nedded

 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  new stdfiles
 *
 * \Return value:   : int -1: error in opening the target file to redirect input
 * 					  int -2: error in duplecating the target file to redirect input
 * 					  int -3: error in closing the target file after stdin pointed to it
 * 					  int -4: error in opening the target file to redirect output
 * 					  int -5: error in duplecating the target file to redirect output
 * 					  int -6: error in closing the target file after stdout pointed to it
 * 					  int -7: error in opening the target file to redirect err
 * 					  int -8: error in duplecating the target file to redirect err
 * 					  int -9: error in closing the target file after stderr pointed to it
 *******************************************************************************/

int external_redirection(command_t* command)
{
	int fd;
	int ret_val =1;
	if(command->stdfiles.in != NULL)
	{
		if((fd= open(command->stdfiles.in, O_CREAT | O_RDWR,0666)) == -1)
		{
			printf("Error opening %s to redirect input.\n", command->stdfiles.in);
			printf("errno = %d\n",errno);
			ret_val=-1;
		}
		if(dup2(fd,0) == -1)
		{
			printf("Error duplicating %s to redirect input.\n", command->stdfiles.in);
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
			printf("Error opening %s to redirect err.\n", command->stdfiles.err);
			printf("errno = %d\n",errno);
			ret_val=-7;
		}
		if(dup2(fd,2) == -1)
		{
			printf("Error duplicating %s to redirect err.\n", command->stdfiles.err);
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
 * \Syntax          : int parser(command_t* command, char* input_line, int* start_index)
 * \Description     : parse only one command from the input line the command starts
 * 					  at the index of start_index and ends with ';' or end of line
 *					  and evaluates the variables if '$' detected
 * \Parameters (in) :char* input_line : the imput line contains the command to be parsed
 * 					 int* start_index : the index of the frist character of the command
 * 					 to be barsed
 *
 * \Parameters (out): command_t* command : pointer to command_t structure to put the
 * 					  command's args and stdfiles in case of redirection in it
 * \Return value:   : int the index of the next command in the line
 *******************************************************************************/
int parser(command_t* command, char* input_line, int* start_index)
{
	binaryFlag_t separator_flag=exist;	//exist when the last char is a separator (space, ';', '\n' or whatever except regular character)
	redirectFlag_t redir_flag = no_redir;	//when redirection exist
	int args_iterator=0;
	int line_iterator;

	//tokenizing the input line
	//parsing only one command in case of multi-command line
	for (line_iterator=(*start_index); (input_line[line_iterator]!=';' && input_line[line_iterator]!='\n' && input_line[line_iterator]!='|' );line_iterator++)
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
		case '\t':	//same as spaces
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
		pipe_flag=not_exist;
	}
	else if(input_line[line_iterator]=='|')
	{
		input_line[line_iterator] ='\0';	//terminating last token be '\0'
		line_iterator+=1;
		pipe_flag=exist;
		pipe_counter++;
	}
	else if(input_line[line_iterator]=='\n')
	{
		input_line[line_iterator]='\0';		//terminating last token be '\0'
		line_iterator++;
		input_line[line_iterator]='\n';		//add a dummy command at the end of all commands '\n' which will be handles as an enter press
		(*(command->ptr_args_arr))[args_iterator]='\0';//terminate the dummy command
		pipe_flag=not_exist;
	}

	//variables evaluating in case of '$' detected
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


/******************************************************************************
 * \Syntax          : int executer(command_t* command)
 * \Description     : execute the parsed command in all cases(internal and external command OR variable definition)
 *					  and do the stdfiles redirection if exist

 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  command's args and stdfiles in case of redirection
 *
 * \Return value:   : int 1 : if success
 * 						 -1	: if error in forking to the new process
 * 						 -2 : if error in parent waiting for the child status
 * 						 -3 : if the child is not terminated normally
 * 						 -4 : error in redirecting stdfiles in child
 * 						 -5 : error in execp function in the child
 *
 * 						 NOTE: in both -4 and -5 return values , the error message will be printed
 * 						 	   in the child process
 *******************************************************************************/

int executer(command_t* command)
{
	int ret_val=1;
	int internal_command_index;
	int val_index;

	//piping redirecting
	if(pipe_flag == exist && pipe_counter ==1) //frist pipe (cmd | something)
	{
		open(pipe_tmp_file1_path, O_TRUNC);
		command->stdfiles.out=pipe_tmp_file1_path;
	}
	else if (pipe_flag == not_exist && pipe_counter !=0)
	{
		if (pipe_counter%2 == 1)//odd
		{
			command->stdfiles.in=pipe_tmp_file1_path;
		}
		else//even
		{
			command->stdfiles.in=pipe_tmp_file2_path;
		}
		pipe_counter=0;
	}
	else if(pipe_flag == exist && pipe_counter%2 == 1)
	{
		open(pipe_tmp_file1_path, O_TRUNC);

		command->stdfiles.in=pipe_tmp_file2_path;
		command->stdfiles.out=pipe_tmp_file1_path;
	}
	else if(pipe_flag == exist && pipe_counter%2 == 0)
	{
		open(pipe_tmp_file2_path, O_TRUNC);

		command->stdfiles.in=pipe_tmp_file1_path;
		command->stdfiles.out=pipe_tmp_file2_path;
	}

	//executing commands
	if((internal_command_index=is_internal_command(command))!=-1)
	{
		if(internal_executer(command, internal_command_index) == -1)
			ret_val =-1;

	}
	else if((val_index=is_variable(command))!=-1)
	{
		if(variable_executer(command, val_index) ==-1)
			ret_val=-2;
	}
	else	//extenal command
	{

		pid_t ret_pid;
		int child_status;
		ret_pid=fork();

		if(ret_pid < 0)//faild
			ret_val =-3;

		else if(ret_pid > 0)//parent
		{
			if(wait(&child_status) == -1)
				ret_val=-4;

			if(!WIFEXITED(child_status))	//child is not terminated normally
				ret_val=-5;
			if(WEXITSTATUS(child_status) == 100)
				ret_val =-6;
			else if (WEXITSTATUS(child_status) == 101)
				ret_val =-7;

		}
		else	//child
		{
			//redirections
			if(external_redirection(command)<0)
			{
				_exit(100);	//return a unique integer to parent
			}

			//exec
			if(execvp((*command->ptr_args_arr)[0],*command->ptr_args_arr) == -1)
			{
				printf("Error executing an external command from execvp().\n");
				printf("errno = %d\n",errno);
				_exit(101);//return a unique integer to parent
			}
		}
	}

	command->stdfiles.in=NULL;
	command->stdfiles.out=NULL;
	command->stdfiles.err=NULL;

	return ret_val;
}




/******************************************************************************
 * \Syntax          : int interpreter(command_t* command, char* input_line)
 * \Description     : parse the input line and execute the commands in the line
 * 					  in case of error in a command all next commands will not be executed
 *
 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  command's args and stdfiles in case of redirection
 * 					  char* input_line	 : the input line which contains the commands to be executed
 *
 * \Return value:   : int 1 : if success
 * 						 -2 : if error when definenig variable due to maximum numbers of variables reached
 * 						 -3	: if error in forking to the new process
 * 						 -4 : if error in parent waiting for the child status
 * 						 -5 : if the child is not terminated normally
 * 						 -6 : error in redirecting stdfiles in child
 * 						 -7 : error in execp function in the child
 *
 * 						 NOTE: in both -6 and -7 return values , the error message will be printed
 * 						 	   in the child process
 *******************************************************************************/

int interpreter(command_t* command, char* input_line)
{
	int ret_val=1;
	int index_new_command=0;
	/*
	 * parsing a line
	 * in case of line contains multi-commands, commands will be parsed
	 * and executed sequentially
	 */
	while(input_line[index_new_command] != '\n')	//loop till the end of line(last command)
	{

		//parser will return every time a single command parsed
		index_new_command=parser(command, input_line, &index_new_command);

		//execute the parsed command
		switch(executer(command))
		{
		case	1:
			break;
		case -2:
			printf("maximum number of variables is reached.\n");
			ret_val =-2;
			break;
		case -3:
			printf("Error while fork to execute external command\n");
			printf("errno = %d\n",errno);
			ret_val =-3;
			break;
		case -4:
			printf("Error while waiting for the child process status\n");
			printf("errno = %d\n",errno);
			ret_val=-4;
			break;
		case -5:
			printf("External command %s is not terminated normally.\n",(*command->ptr_args_arr)[0]);
			ret_val=-5;
			break;
		case -6://error redirecting childs stdfiles
			ret_val =-6;
			break;
		case -7://error in execvp in child
			ret_val = -7;
			break;
		}

		if(ret_val<0)
			break;	//don't execute the remained commands in the line
	}
	return ret_val;
}

/**********************************************************************************************************************
 *  END OF FILE: arguments.c
 *********************************************************************************************************************/
