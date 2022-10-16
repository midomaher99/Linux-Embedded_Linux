./executable/shelldemo: ./src/*.c ./inc/*.h
	mkdir -p executable
	gcc -o ./executable/shelldemo ./src/*.c -I ./inc -ggdb -Wall
	@echo =========build is done=========
clean: 
	rm -r executable
