/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File: fib.c
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
 * \Syntax          : void printFibSeq(int number)
 * \Description     : takes a number as an input and calcs the fib sequence and print the seq from the second element (zero and one is not printed)
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Reentrant
 * \Parameters (in) : int number: the number required to print the fib sequence for
 * \Parameters (out): None
 * \Return value:   : None 
 *******************************************************************************/
void printFibSeq(int number)
{
	int fib=0;
	int last = 1;	//hold the last element in the series
	int beforeLast =0; //hold the element before the last element in the series
	int i;
	for(i=1; i<number; i++)
	{
		fib= last+ beforeLast;
		beforeLast = last;
		last = fib;
		printf(", %d", fib);
	}
	printf("\n");

}
/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : void fibCommand(void)
 * \Description     : takes a number from user and prints the fibonacci sequence for this nmber
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : None 
 *******************************************************************************/
void fibCommand(void)
{

	int number=0;
	char x;
	printf("Shell> Enter a number: ");
	scanf("%d",&number);
	scanf("%c",&x);
	printf("Shell> fib sequence is: %d, %d", 0, 1);
	printFibSeq(number);
	
}
/**********************************************************************************************************************
 *  END OF FILE: fib.c
 *********************************************************************************************************************/
