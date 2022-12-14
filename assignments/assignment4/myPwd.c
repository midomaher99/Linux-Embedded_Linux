/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  myPwd.c
 *  
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/*********************************************************************** ***********************************************
 *  LOCAL MACROS CONSTANT\FUNCTION
 *********************************************************************** **********************************************/
#define fd_stdin (1U)

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/
int main()
{
    char *buff = NULL;
    /*first parameter (buf) is equal to  null to be allocated internally inside getcwd()
       and the other parameter (size) is 0 to allocate buff with suitable size can contain the returned path 
     */

    while ((buff = getcwd(NULL, 0)) == NULL);	//if there is any error recall the function

    write(fd_stdin, buff, strlen(buff));
	write(fd_stdin, "\n", 1);

	// freeing buff due to instructions in (man getcwd)
    free(buff);
    return 0;
}

/**********************************************************************************************************************
 *  END OF FILE: myPwd.c
 *********************************************************************************************************************/
