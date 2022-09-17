# Output
![image](https://user-images.githubusercontent.com/41878952/190841303-8167e438-3a29-4a5b-8257-80d172e3006c.png)

# Used commands

## For static library
### Creating relocatable object files
* gcc -c -g femtoShell.c -I includes/ -o objects/static/femtoShell.o 
* gcc -c -g source/fact.c -o objects/static/fact.o 
* gcc -c -g source/rand.c -o objects/static/rand.o 
* gcc -c -g source/fib.c -o objects/static/fib.o 
### Creating the static library
* ar rcs objects/static/libCommands.a objects/static/fact.o objects/static/rand.o objects/static/fib.o 
### Creating statically linked executable
* gcc objects/static/femtoShell.o  objects/static/libCommands.a -o objects/execuables/myFemtoShellStatic

## For dynamic library
### Creating relocatable object files
* gcc -c -g femtoShell.c -I includes/ -o objects/dynamic/femtoShell.o 
* gcc -c -g -fpic source/fact.c -o objects/dynamic/fact.o 
* gcc -c -g -fpic source/rand.c -o objects/dynamic/rand.o 
* gcc -c -g -fpic source/fib.c -o objects/dynamic/fib.o 
### Creating the Dynamic library
* gcc -shared  objects/dynamic/fact.o objects/dynamic/rand.o objects/dynamic/fib.o -o objects/dynamic/libCommands.so
### Creating dynamically linked executable
* gcc objects/dynamic/femtoShell.o -L objects/dynamic/ -lCommands -o objects/execuables/myFemtoShellDynamic
### Adding the library path to the environment variables
* export LD_LIBRARY_PATH=./objects/dynamic:$LD_LIBRARY_PATH
