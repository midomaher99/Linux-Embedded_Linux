/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File: local_vars.c
 *  
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "loc_vars.h"
#include "config.h"
#include "stdio.h"
#include "commands.h"
#include "stdlib.h"
#include "string.h"

/**********************************************************************************************************************
 *  LOCAL DATA
 *********************************************************************************************************************/
int variables_counter=0;

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/
locVariable_t* loc_vars_array[NUMBER_OF_VARIABLES]={NULL};

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : char *getloc(const char *name)
 * \Description     : similar to getenv function gets the local variable value
 *
 * \Parameters (in) : char *name : the name of the required variable
 * \Return value:   : char* : the value of the variable in case of found in loc_vars_array and NULL is not found
 *******************************************************************************/

char *getloc(char *name)
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
int is_variable(command_t* command)
{
	int ret_val=-1;
	for(int i =0;((*command->ptr_args_arr)[0])[i] != '\0';i++)	// iterate on the first token characters till end of token
	{
		if(((*command->ptr_args_arr)[0])[i] == '=')
		{
			ret_val=i+1;	//the index of the value
			((*command->ptr_args_arr)[0])[i] = '\0';	//separate between var name and value
			break;
		}
	}
	return ret_val;
}

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
int variable_executer(command_t* command,int val_index)
{
	int ret_val=1;

	if (variables_counter>=NUMBER_OF_VARIABLES)
		ret_val =-1;

	else
	{
		int i;
		for (i=0;i!=variables_counter &&strcmp(loc_vars_array[i]->var, (*command->ptr_args_arr)[0]); i++);

		if (i== variables_counter)
		{
		loc_vars_array[variables_counter]=(locVariable_t*)malloc(sizeof(locVariable_t));
		loc_vars_array[variables_counter]->var=(char*)malloc(strlen((*command->ptr_args_arr)[0])*sizeof (char));
		strcpy(loc_vars_array[variables_counter]->var, (*command->ptr_args_arr)[0]);
		loc_vars_array[variables_counter]->val=(char*)malloc(strlen(((*command->ptr_args_arr)[0])+val_index)*sizeof (char));
		strcpy(loc_vars_array[variables_counter]->val, ((*command->ptr_args_arr)[0])+val_index);
		variables_counter++;
		}
		else
		{
			strcpy(loc_vars_array[i]->val, ((*command->ptr_args_arr)[0])+val_index);
		}
	}
	return ret_val;
}


/**********************************************************************************************************************
 *  END OF FILE: local_vars.c
 *********************************************************************************************************************/
