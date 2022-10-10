/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  arguments.c
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <stdlib.h>

/**********************************************************************************************************************
 *  LOCAL MACROS CONSTANT\FUNCTION
 *********************************************************************************************************************/
#define ARGS_ARRAY (**args)
#define PTR_ARGS_ARRAY (*args)


/**********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/
typedef enum
{
	not_exist,
	exist
}flag_t;

/**********************************************************************************************************************
 *  LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/

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
 *					  			 ^		  ^		   ^		^		   ^				^
 *					  			 |________| _______| _______|__________|________________|_____________
 * 					 		    ||		 ||  |	  ||  |	   ||		  ||		  |	   ||	|	 |	  |
 * 	 					   ARGS:||       ||  |'\0'||  |'\0'||	 	  ||          |'\0'||   |'\0'|'\n'|
 *					   		    |________|___|____|___|____|__________|___________|____|____|____|____|
 *
 *
 * \Parameters (in) : char* line 	   : the input from the user the line is '\n' terminated
 * 					  ssize_t length   : the input line length + 2
 * \Parameters (out): char** (**args)[]: will be dynamically allocated to contain the commands' arguments
 * 										 must be freed after calling the function
 * \Return value:   : None
 *******************************************************************************/

void extract(char* line, ssize_t length, char** (**args)[])
{
	flag_t separator_flag=exist;	//exist when the last char is a separator (space, ';', '\n' or whatever except regular character)

	int line_iterator;	//iterates on the input
	int args_iterator=0;

	PTR_ARGS_ARRAY=(char**(*)[])malloc(length*sizeof(char**));	//length is for worst case and will be reallocated later
	
	//extracting method is shown in a flow chart in README.md file
	for (line_iterator=0;line_iterator<(length);line_iterator++)	//loop on the input
	{
		if(line[line_iterator]== ' ')
		{
			if(separator_flag == exist)
			{
				//do nothing
			}
			else//flag not exist
			{
				separator_flag=exist;
				line[line_iterator]='\0';
			}
		}

		else if(line[line_iterator]== ';')
		{
			if(separator_flag == exist)
			{
				ARGS_ARRAY[args_iterator] = '\0';
				args_iterator++;
			}
			else//flag not exist
			{
				separator_flag = exist;
				line[line_iterator]='\0';
				ARGS_ARRAY[args_iterator]='\0';
				args_iterator++;
			}
		}

		else if(line[line_iterator]== '\n')
		{
			if(separator_flag == exist)
			{
				ARGS_ARRAY[args_iterator]=&line[line_iterator];
				args_iterator++;
				ARGS_ARRAY[args_iterator]='\0';
				line_iterator++;
				line[line_iterator]='\0';
			}
			else//flag not exist
			{
				line[line_iterator]='\0';
				line_iterator++;
				line[line_iterator]='\n';
				ARGS_ARRAY[args_iterator]='\0';
				args_iterator++;
				ARGS_ARRAY[args_iterator]=&line[line_iterator];
				line_iterator++;
				args_iterator++;
				line[line_iterator]='\0';
				ARGS_ARRAY[args_iterator]='\0';
			}
			args_iterator++;
			ARGS_ARRAY[args_iterator]='\n';	//to know that this is the last command
		}

		else
		{
			if(separator_flag == exist)
			{
				ARGS_ARRAY[args_iterator]=&line[line_iterator];
				args_iterator++;
				separator_flag = not_exist;
			}
			else//flag not exist
			{
				//do nothing
			}	
		} 
		
	}

	args_iterator++;	// ++ as ARGS_ARRAY is zero indexed but realloc() needs the absolute number of elements
	PTR_ARGS_ARRAY=(char**(*)[])realloc(PTR_ARGS_ARRAY,(size_t)args_iterator*sizeof(char**)); //reallocating ARGS_ARRAY to avoid over sizing
}




/**********************************************************************************************************************
 *  END OF FILE: arguments.c
 *********************************************************************************************************************/
