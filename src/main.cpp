#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>

using namespace std;

void loginInfo(char*& login, char*& hostname, size_t& hostLength)
{
	passwd* loginDetails = getpwuid(getuid());
	login = loginDetails->pw_name;
	int gethost = gethostname(hostname, hostLength);
	if (gethost == -1)
	{
		perror("Error retrieving hostname");
		exit(1);
	}
	if (login == NULL)
	{
		perror("Failed to retrieve login ID");
		exit(1);
	}
}

void parseSpace (char* userinput, char** argv)
{
	char delims [] = " ";
	char* token = strtok(userinput, delims);
	for (int i = 0; token != NULL; i++)
	{
		argv[i] = token;
		token = strtok(NULL, delims);
	}
}

void parseSemiAND (char* userinput, char** argv)
{
	char delims [] = ";&";
	char* token = strtok(userinput, delims);
	for (int i = 0; token != NULL; i++)
	{
		argv[i] = token;
		token = strtok(NULL, delims);
	}
}

void parseComment (char* userinput, char** argv)
{
	char delims [] = "#";
	char* token = strtok(userinput, delims);
	for (int i = 0; token != NULL; i++)
	{
		argv[i] = token;
		token = strtok(NULL, delims);
	}
}

void parseOR (char* userinput, char** argv)
{
	char delims [] = "|";
	char* token = strtok(userinput, delims);
	for (int i = 0; token != NULL; i++)
	{
		argv[i] = token;
		token = strtok(NULL, delims);
	}
}

void parseAND (char* userinput, char** argv)
{
	char delims [] = "&";
	char* token = strtok(userinput, delims);
	for (int i = 0; token != NULL; i++)
	{
		argv[i] = token;
		token = strtok(NULL, delims);
	}
}

void isExit(char* input)
{
	if (strcmp(input, "exit") == 0)
	{
		exit(0);
	}
}

void checkFork(int pid, char** argv)
{
	if (pid == -1)	//Detects for error in fork
	{
		perror("An error has occurred with fork");
		exit(1);
	}
	else if (pid == 0)	//This is the child process
	{
		execvp(argv[0], argv);
		perror("An error has occured with execvp");
		exit(1);
	}
	else if (pid > 0)	//This is the parent process
	{
		if (wait(NULL) == -1)	//Checks for error
		{				//In waitpid
			perror("An error has occurred with waitpid");
			exit(1);
		}
	}
}

int main()
{
	
	cout << "Initializing RSHELL" << endl;
	char*** argv1 = new char**[1024]; //Container holding all char** command lists
	char** argv = new char*[1024];	
	char** checkComment = new char*[1024]; 	//Used for parsing if there's a comment
	char** checkOR = new char*[1024];
	char* hostname = new char[1024];
	char* userinput = new char [1024];
	char* login = '\0';
	size_t hostLength = 4096;

	memset(argv1, 0, 1024);
	memset(argv, 0, 1024);
	memset(checkComment, 0, 1024);
	memset(checkOR, 0, 1024); 
	memset(hostname, 0, 1024);
	memset(userinput, 0, 1024);
	do
	{
		loginInfo(login, hostname, hostLength);
		int forkID = 0;
		cout << login << "@" << hostname << "$ ";
		cin.getline(userinput, 1024, '\n');
		parseComment(userinput, checkComment);
		parseOR(checkComment[0], checkOR);
		for (int i = 0; checkOR[i] != NULL; i++)
		{
			parseSemiAND(checkOR[i], argv);
		}
		for (int i = 0; argv[i] != NULL; i++)
		{
			argv1[i] = new char*[1024];
			memset(argv1[i], 0, 1024);
			parseSpace(argv[i], argv1[i]);
		}

		for (int i = 0; argv1[i] != NULL; i++)
		{
			isExit(argv1[i][0]); 
			forkID = fork();
			checkFork(forkID, argv1[i]);
		}
		memset(argv1, 0, 1024);
		memset(argv, 0, 1024);
		memset(checkComment, 0, 1024);
		memset(checkOR, 0, 1024); 
		memset(hostname, 0, 1024);
		memset(userinput, 0, 1024);
		
		/*memContainer(argv1);
		mem2DArray(argv);
		mem2DArray(checkComment);
		mem2DArray(checkOR);
		memArray(hostname);
		memArray(userinput);
		memArray(login);	*/
	} while (true);

	for (int i = 0; argv1[i] != NULL; i++)
	{
		delete [] argv1[i];
	}
	delete [] checkComment;
	delete [] hostname;
	delete [] userinput;
	delete [] argv;
	delete [] argv1;
	delete [] checkOR;

	return 0;
}
