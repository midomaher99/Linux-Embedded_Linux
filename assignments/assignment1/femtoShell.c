#include "stdio.h"
#include "string.h"
int main (void)
{
	char buffer[100];

	while(1)
	{
		printf("Maher > ");
		fgets( buffer, 100,stdin);
		if(strcmp(buffer, "exit\n"))
			printf("Shell: %s",buffer);
		else
		{
			printf("Good Bye :) \n");
			break;
		}
	}

	return 0;
}
