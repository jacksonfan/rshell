compiler=g++
flags=-Wall -Werror -ansi -pedantic

all:	bin\
	rshell\
	ls\
	cp

bin:	
	[ ! -d bin ]
	mkdir -p bin;

rshell: src/rsig.cpp
	
	$(compiler) $(flags) src/rsig.cpp -o bin/rshell 

ls:	src/ls.cpp
	
	$(compiler) $(flags) src/ls.cpp -o bin/ls

cp:	src/cp.cpp
	
	$(compiler) $(flags) src/cp.cpp -o bin/cp

clean:	
	rm -rf bin


