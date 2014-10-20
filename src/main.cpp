#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include <stdlib.h>

using namespace std;

int main()
{
	
	cout << "Initializing RSHELL" << endl;
	// Add specific login info later (for ec)
	char* userinput = new char[200];
	char** argv = new char*[50];
	char exitcom[] = "exit";
	do
	{
		cout << "$ ";
		cin.getline(userinput, 200, '\n');
		char* tokStore = strtok(userinput, " ");
		for (int i = 0; tokStore != NULL; i++) //Parses
		{				       //And stores
			argv[i] = tokStore;	       //Input into
			tokStore = strtok(NULL, " ");  //Argv
		}				       
		if (strcmp(argv[0],exitcom) == 0)  //If user inputted exit
		{				   //Break out of loop (shell)
			break;	
		}
		int forkID = fork();
		if (forkID == -1)	//Detects for error in fork
		{
			perror("An error has occurred with fork");
			exit(1);
		}
		else if (forkID == 0)	//This is the child process
		{
			execvp(argv[0], argv);
			perror("An error has occured with execvp");
			exit(1);
		}
		else if (forkID > 0)	//This is the parent process
		{
			if (wait(0) == -1)	//Checks for error
			{				//In waitpid
				perror("An error has occurred with waitpid");
				exit(1);
			}
		}
		memset(userinput, 0, 200); //Set all to 0 for fresh input
		memset(argv, 0, 50);	   //Set all to 0 for fresh args
	} while (true);

	delete [] userinput;
	delete [] argv;
	cout << "Exiting shell..." << endl;









	return 0;
}
