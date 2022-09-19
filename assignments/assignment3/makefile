#only can take static or dynamic
#make sure that there is no spaces or tabs after or before the set value
LINKING_TYPE=dynamic

######################################
SOURCE_PATH=./source
EXE_PATH=./objects/executables
INC= -I ./includes


ifeq ($(LINKING_TYPE), static)
OBJ_PATH=./objects/static
LIB=libCommands.a
else ifeq ($(LINKING_TYPE), dynamic)
OBJ_PATH=./objects/dynamic
LIB=libCommands.so

endif

#####################################
$(EXE_PATH)/myFemtoShell: $(OBJ_PATH)/femtoShell.o $(OBJ_PATH)/$(LIB)
ifeq ($(LINKING_TYPE), static)
	gcc -g $^  -o $@
else ifeq ($(LINKING_TYPE), dynamic)
	gcc -g $< -lCommands -L $(OBJ_PATH) -o $@
	export LD_LIBRARY_PATH=$(OBJ_PATH):$$LD_LIBRARY_PATH

endif
	@echo "##############BUILD IS DONE##############"

$(OBJ_PATH)/femtoShell.o: femtoShell.c
	echo "$(OBJ_PATH)"
	gcc -c -g $(INC) $< -o $@

$(OBJ_PATH)/$(LIB): $(OBJ_PATH)/fib.o $(OBJ_PATH)/fact.o $(OBJ_PATH)/rand.o
ifeq ($(LINKING_TYPE), static)
	ar rcs $@ $^
else ifeq ($(LINKING_TYPE), dynamic)
	gcc -shared $^ -o $@
endif


$(OBJ_PATH)/%.o:$(SOURCE_PATH)/%.c
ifeq ($(LINKING_TYPE), static)
	gcc -c -g $< -o $@
else ifeq ($(LINKING_TYPE), dynamic)
	gcc -c -g -FPIC $< -o $@
endif

clean:
	rm $(OBJ_PATH)/*.o

clean_all:
	rm $(OBJ_PATH)/* $(EXE_PATH)/*
