/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  myCp.c
 *  
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


/**********************************************************************************************************************
*  LOCAL MACROS CONSTANT\FUNCTION
*********************************************************************************************************************/
#define BUFFER_SIZE (100UL)

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/
int main(int argc, char *argv[])
{
    if (argc < 3) {
	printf("Too few arguments\n");
	return 1;
    } else if (argc < 3) {
	printf("Too much arguments\n");
	return 2;
    } else {

	int fd_src;
	int fd_dest;
	int read_count = 1;
	char buffer[BUFFER_SIZE];

	fd_src = open(argv[1], O_RDONLY);

	if (fd_src == -1) {
	    printf("Error in opening %s.\n", argv[1]);
	    printf
		("check the return value for the errno by using \"echo $?\"\n");
	    return errno;

	}

	fd_dest = open(argv[2], O_CREAT | O_WRONLY, 0666);	// read write permission for user, group and others

	if (fd_dest == -1) {
	    printf("Error in creating %s.\n", argv[2]);
	    printf
		("check the return value for the errno by using \"echo $?\"\n");
	    close(fd_src);
	    return errno;
	}

	while (read_count > 0) {

	    read_count = read(fd_src, buffer, BUFFER_SIZE);

	    if (read_count == -1)	//read returns -1 in case of error
	    {
		printf("Error in reading %s.\n", argv[1]);
		printf
		    ("check the return value for the errno by using \"echo $?\"\n");
		close(fd_src);
		close(fd_dest);
		return errno;
	    } else {
		if (write(fd_dest, buffer, read_count) == -1)	//write returns -1 in case error
		{
		    printf("Error in writing %s.\n", argv[2]);
		    printf
			("check the return value for the errno by using \"echo $?\"\n");
		    close(fd_src);
		    close(fd_dest);
		    return errno;

		}
	    }
	}
	close(fd_src);
	close(fd_dest);
    }
    return 0;
}

/**********************************************************************************************************************
 *  END OF FILE: myCp.c
 *********************************************************************************************************************/
