compiler=g++
flags=-Wall -Werror -ansi -pedantic

all:	bin\
	rshell\
	ls\
	cp

bin:	
	[ ! -d bin ]
	mkdir -p bin;

rshell: src/rshell2.cpp
	
	$(compiler) $(flags) src/rshell2.cpp -o bin/rshell 

ls:	src/ls2.cpp
	
	$(compiler) $(flags) src/ls2.cpp -o bin/ls

cp:	src/cp.cpp
	
	$(compiler) $(flags) src/cp.cpp -o bin/cp

clean:	
	rm -rf bin


