
compiler = "g++"
flags =-Wall -Werror -ansi -pedantic

all:	rshell 

rshell: src/main.cpp
	[ ! -d bin ]
	mkdir -p bin; 
	
	$(compiler) $(flags) src/main.cpp bin/rshell 

