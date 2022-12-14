/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File: fact.c
 *        Brief:  -
 *
 *  Description:  - 
 *  
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <stdio.h>

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : static int calcFactorial(int number)
 * \Description     : calculate the factorial of a number
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Reentrant
 * \Parameters (in) : int number:	the number required to calc ints factorial
 * \Parameters (out): None
 * \Return value:   : int factorial:	the factorial of the input number	
 *******************************************************************************/

static int calcFactorial(int number)
{
	int fact=0;
	fact = number;
	
	while(number > 2)
	{
		fact *= (number -1);
		number -=1;	
	}
	return fact;
}

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : void factCommand(void)
 * \Description     : takes a number from user and print the factorial of this number
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : None
 *******************************************************************************/
void factCommand(void)
{
	int number=0;
	int factorial =0;
	char x;
	printf("Shell> Enter a number: ");
	scanf("%d",&number);
	scanf("%c",&x);
 	factorial = calcFactorial(number);
	printf("Shell> factorila is: %d\n", factorial);
}
/**********************************************************************************************************************
 *  END OF FILE: fact.c
 *********************************************************************************************************************/
