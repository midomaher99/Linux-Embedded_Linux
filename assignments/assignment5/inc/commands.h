/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  commands.c
 *********************************************************************************************************************/

#ifndef COMMANDS_H
#define COMMANDS_H

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/

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
	char* command_name;
	int(*ptr_command)(int argc, char*argv[]);	//the command function
}internalcommand_t;

 
/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : int internal_executer(command_t*  command,int internal_command_index)
 * \Description     : executes the given internal command
 *

 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  command's args
 * 					  int internal_command_index: the command index in internal_commands_array

 * \Return value:   : int 1
 *******************************************************************************/
extern int internal_executer(command_t*  command,int internal_command_index);

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
extern int is_internal_command(command_t* command);

 
#endif  /* FILE_NAME_H */

/**********************************************************************************************************************
 *  END OF FILE: Std_Types.h
 *********************************************************************************************************************/




