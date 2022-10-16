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
1. executing external command `ls`
2. defining local variable 'x=-l'
