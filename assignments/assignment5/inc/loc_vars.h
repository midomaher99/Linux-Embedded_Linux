/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  loc_var.h
 *********************************************************************************************************************/

#ifndef LOC_VAR_H
#define LOC_VAR_H
/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/
#include "commands.h"

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/

typedef struct
{
	char* var;
	char* val;
}locVariable_t;

/**********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 *********************************************************************************************************************/

extern locVariable_t* loc_vars_array[256];

 
/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : char *getloc(const char *name)
 * \Description     : similar to getenv function gets the local variable value
 *
 * \Parameters (in) : char *name : the name of the required variable
 * \Return value:   : char* : the value of the variable in case of found in loc_vars_array and NULL is not found
 *******************************************************************************/
extern char *getloc(char *name);


/******************************************************************************
 * \Syntax          : int is_variable(command_t* command)
 * \Description     : determine if the given command is a variable definition or not
 *
 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  command's args
 *
 * \Return value:   : int -1 : in case of not a variable definition
 * 					  the index of the frist character of the value in case of variable defenition
 *******************************************************************************/
extern int is_variable(command_t* command);

/******************************************************************************
 * \Syntax          : int variable_executer(command_t* command,int val_index)
 * \Description     : executes the variable definition command
 *
 * \Parameters (in) : command_t* command : pointer to command_t structure which contain
 * 					  command's args
 *
 * \Return value:   : int 1 : if success
 * 						 -1 : if the muximum number defined in config.h reached and unable to add other variables
 *
 *******************************************************************************/
extern int variable_executer(command_t* command,int val_index);
 
#endif  /* LOC_VARS_H*/

/**********************************************************************************************************************
 *  END OF FILE: loc_Vars.h
 *********************************************************************************************************************/



