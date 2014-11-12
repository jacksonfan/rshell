rshell
======

Author and Contributors
---------
Jackson Fan


Installing and Removing the Program
----------

1. Open a bash terminal
2. Clone my repository by typing 
'git clone https://github.com/jfan013/rshell.git'
3. Now, enter the repository using 'cd rshell'
4. Type 'make'
5. The code is now compiled. Type 'bin/' followed by the command name
to run the command Ex: 'bin/rshell'

* To remove the program, run 'make clean'

BUGS
===

rshell
-----

* && operator will run every function on the line, regardless of failure
* || does not run properly. It executes the last input only 
* Has an upper limit of 500 commands
* Program does not differentiate between && and & or || and |
* A lot of commented out code, causing readability issues

ls
---

* Spacing on output has issues for each flag
* No color has been implemented
* Has a set amount of files per line (Is not dynamic)
* Does not sort exactly according to ls
* Did not implement total blocks in -l flag


