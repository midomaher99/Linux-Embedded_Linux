# Assignment 5
Making a simple shell executes external commands, support some internal commands and other features.

## Internal commands
* mycd [directory path] : to change the current working directory to the given one. Note: the pase can be absolute or relative.
* myhistory : to show the commands history.
* myset : to list all local variables defined.
* myexport [variable name] : to export the given local variable to the environment variables.
* mypwd : to print the path of the current working directory.

## Features
* Can execute multi commands separated be ';' in the same line.
* Can redirect input and output of *external commands ONLY* by using the '>' and '<' operators.
* Can define local variables using *variable=value* command without spaces.
* Can evaluate local and environment variables if a variable passed as a command argument using '$' operator.
* If '$' operator is used with a non-exist variable it will be just ignored.
