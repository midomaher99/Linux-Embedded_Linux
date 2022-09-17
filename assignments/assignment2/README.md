# Output
![image](https://user-images.githubusercontent.com/41878952/190840881-ed15ef9b-20af-453f-81be-aaea6584fada.png)

# Used commands

## For static library
### Creating relocatable object files
* gcc -c -g femtoShell.c -I includes/ -o objects/static/femtoShell.o 
* gcc -c -g source/fact.c -o objects/static/fact.o 
* gcc -c -g source/rand.c -o objects/static/rand.o 
* gcc -c -g source/fib.c -o objects/static/fib.o 
### Creating static library
* ar rcs objects/static/libCommands.a objects/static/fact.o objects/static/rand.o objects/static/fib.o 
### Creating statically linked executable
* gcc objects/static/femtoShell.o  objects/static/libCommands.a -o objects/execuables/myFemtoShellStatic
