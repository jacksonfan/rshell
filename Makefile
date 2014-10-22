
opt = "-Wall -Werror -ansi -pedantic"
compiler = "g++"

all:	
	rshell

rshell:
	[ ! -d bin ];
	then mkdir -p bin; 
	fi
	$(compiler) $(opt) src/main.cpp -o bin/rshell 

clean:
	rm bin/*.o bin/rshell.out
