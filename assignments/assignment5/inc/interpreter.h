/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  <Write File Name>
 *       Module:  -
 *
 *  Description:  <Write File DESCRIPTION here>     
 *  
 *********************************************************************************************************************/
#ifndef INTERPRETER_H
#define INTERPRETER_H
/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/
#include "commands.h"

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

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
extern int interpreter(command_t* command, char* input_line);

 
#endif  /* COMMANDS_H */

/**********************************************************************************************************************
 *  END OF FILE: commands.h
 *********************************************************************************************************************/
