#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


int main()
{
    char *buff = NULL;
    /*first parameter (buf) is equal to  null to be allocated internally inside getcwd()
       and the other parameter (size) is 0 to allocate buff with suitable size can contain the returned path 
     */

    while ((buff = getcwd(NULL, 0)) == NULL);	//if there is any error recall the function


    printf("%s\n", buff);
    // freeing buff due to instructions in (man getcwd)
    free(buff);
    return 0;
}
