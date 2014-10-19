#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <sstream>

using namespace std;

int main()
{
	
	cout << "Initializing RSHELL" << endl;
	// Add specific login info later (for ec)
	string userinput;
	char* argv[] = new char*[20];
	while (argv[0] != "exit")
	{
		cout << "$ ";
		istringstream parse;
		getline(cin, parse);
		for (int i = 0; !parse.eof(); i++)
		{
			parse >> argv[i];
		}
		if (argv[0] == "exit")
		{
			cout << "Now exiting RSHELL" << endl;
			exit(0);
		}
		int forkID = fork();
		if (forkID == -1)
		{
			perror("Error resulted from fork");
			exit(1);
		}
		else if (forkID == 0) //Child process
		{
			execvp(argv[0], argv);
		}
		else if (forkID == 1) //Parent process
		{
			if (wait(NULL) == -1)
			{
				perror("Error resulted from wait()
					parent process");
				exit(1);
			}
		}
			
	}
















	return 0;
}
