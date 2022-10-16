# Assignment 5
Making a simple shell executes external commands, supports some internal commands and other features.

## Internal commands
* mycd [directory path] : to change the current working directory to the given one. Note: the pase can be absolute or relative.
* myhistory : to show the commands history.
* myset : to list all local variables defined.
* myexport [variable name] : to export the given local variable to the environment variables.
* mypwd : to print the path of the current working directory.
* myexit : to terminate the shelldemmo.

## Features
* Can execute multi commands separated be ';' in the same line.
* Can redirect input and output of commands by using the '>' and '<' operators.
* Can define local variables using **variable=value** command without spaces, ex: x=5.
* Can evaluate local and environment variables if a variable passed as a command argument using '$' operator.
* Can handle piping using '|' operator, ex: cmd1 | command2.
* Support multi-piping, ex: cmd1 | cmd2 | cmd3.
* If '$' operator is used with a non-exist variable it will be just ignored.

## shellDemmo vs bash-shell
![Screenshot from 2022-10-17 01-11-47](https://user-images.githubusercontent.com/41878952/196063481-e9a160ee-c02b-4a1a-bc74-119b7f7dd548.png)
1. executing external command `ls`.
2. defining local variable `x=-l`.
3. evaluating the local variable value using `$` operator.
4. redirecting output using `>` operator.
5. piping output using `|` operator.
    ![Screenshot from 2022-10-17 01-26-45](https://user-images.githubusercontent.com/41878952/196063827-dda0563e-bd72-4b89-992a-4249777c3009.png)
6. using `myset` internal command with piping to `less` to see local variables
  ![Screenshot from 2022-10-17 00-57-30](https://user-images.githubusercontent.com/41878952/196064049-d3afc3dc-463b-4929-8c3d-210430bed415.png)
7. Using `mycd` command to change directory and verfy it by `mypwd` command to the the directory change.
8. using `myexport` command to pass th `x` local variable to environmens variables.
9. verfy this by calling a child bash and `echo` the value of `x` using `$` operator.
10. evaluating environment variables using `$` operator.
11. exectes multi-commands in the same line using `;` operator.
