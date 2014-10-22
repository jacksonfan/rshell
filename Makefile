
opt = "-Wall -Werror -ansi -pedantic"
compiler = "g++"

all:
	$(compiler) $(opt)

rshell:
	$(compiler) $(opt)
