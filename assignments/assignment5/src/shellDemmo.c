#include "prompt.h"
#include "stdio.h"
int main ()
{
	int status;
	status=print_prompt();
	
	// chech the return value of print_prompt
	switch (status)
	{
		case 0 :
			break;

		case USER_NO_MATCH:
			printf("Error getting the username.\n");
			printf("No match for \"USERNAME\" environment variable.\n");
			return -1;
			break;

		default:
		
			printf("Error getting the path of the current working directory.\n");
			printf("errno = %d\n",status);
			return -1;
			break;

	}

	getchar();





	return 0;
}
