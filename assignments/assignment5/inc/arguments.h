/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  arguments.h
 *
 *********************************************************************************************************************/
#ifndef ARGUMENTS_H
#define ARGUMETNS_H










/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
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
 *					  			 ^		  ^		   ^		^		   ^
 *					  			 |________| _______| _______|__________|_________________________
 * 					 		    ||		 ||  |	  ||  |	   ||		  ||		  |	   |	|	 |
 * 	 					   ARGS:||       ||  |'\0'||  |'\0'||	 	  ||          |'\0'|'\n'|'\0'|
 *					   		    |________|___|____|___|____|__________|___________|____|____|____|
 *
 *
 * \Parameters (in) : char* line 	   : the input from the user the line is '\n' terminated
 * 					  ssize_t length   : the input line length + 2
 * \Parameters (out): char** (**args)[]: will be dynamically allocated to contain the commands' arguments
 * 										 must be freed after calling the function
 * \Return value:   : None
 *******************************************************************************/

extern void extract(char* line, ssize_t length, char* (**args)[]);
//extern void executer(&command);
#endif  /* ARGUMENTS_H */

/**********************************************************************************************************************
 *  END OF FILE: arguments.h
 *********************************************************************************************************************/
