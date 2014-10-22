
compiler = "g++"
opt =-Wall -Werror -ansi -pedantic

all:	rshell

rshell: src/main.o
	[ ! -d bin ]
	mkdir -p bin; 
	
	$(compiler) $(opt) src/main.cpp -o bin/rshell 

main.o: main.cpp
	$(compiler) $(opt) src/main.cpp 

clean:
	rm bin/*.o bin/rshell.out
