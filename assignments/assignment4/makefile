all: ./executables/myPwd ./executables/myCp ./executables/myMv ./executables/myEcho
	@echo ==================Building Done==================
./executables/%: %.c
	mkdir -p executables
	gcc $< -o $@
