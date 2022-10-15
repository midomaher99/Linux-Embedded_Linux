/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  <Write File Name>
 *       Module:  -
 *
 *  Description:  <Write File DESCRIPTION here>     
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
 *  LOCAL MACROS CONSTANT\FUNCTION
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL DATA
 *********************************************************************************************************************/
int variables_counter=0;

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/
locVariable_t* loc_vars_array[NUMBER_OF_VARIABLES]={NULL};

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
 * \Syntax          : Std_ReturnType FunctionName(AnyType parameterName)
 * \Description     : Describe this service
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : parameterName   Parameter Describtion
 * \Parameters (out): None
 * \Return value:   : Std_ReturnType  E_OK
 *                                    E_NOT_OK
 *******************************************************************************/

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


int variable_executer(command_t* command,int val_index)
{
	int ret_val=1;

	if (variables_counter>=NUMBER_OF_VARIABLES)
	{
		printf("maximum number of variables is reached.\n");
		ret_val =-1;
	}
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
 *  END OF FILE: FileName.c
 *********************************************************************************************************************/
