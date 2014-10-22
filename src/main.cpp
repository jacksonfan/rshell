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

//int 
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
	//return 0;
}

int main()
{
	
	cout << "Initializing RSHELL" << endl;
	char*** argv1 = new char**[500]; //Container holding all char** lists of commands
	//char*** storeOR = new char**[50];
	char** argv = new char*[500];	//Maximum commands allowed = 50
	char** checkComment = new char*[500]; //Used for parsing if there's a comment
	char** checkOR = new char*[500];
	char* hostname = new char[500];	//Largest hostname allowed = 100 chars
	char* userinput = new char [500];//Largest input allowed = 200 chars 
	//char* copyOfInput = '\0';
	char* login = '\0';
	size_t hostLength = 500;	//Largest hostname allowed = 100 bytes
	//int useOR = 0;
	do
	{
		loginInfo(login, hostname, hostLength);
		int forkID = 0;
		cout << login << "@" << hostname << "$ ";
		cin.getline(userinput, 500, '\n');
		parseComment(userinput, checkComment);
		//copyOfInput = strdup(checkComment[0]);	
		parseOR(checkComment[0], checkOR);
		/*if (copyOfInput != checkOR[0])
		{
			for (int i = 0; checkOR[i] != NULL; i++)
			{
				storeOR[i] = new char*[50];
				parseSemiAND(checkOR[i], storeOR[i]);
			}
			for (int i = 0; storeOR[i] != NULL; i++)
			{
				for (int j = 0; (storeOR[i])[j] != NULL; j++)
				{
					argv1[i] = new char*[50];
					parseSpace((storeOR[i])[j], argv1[i]);
				}
			}
			useOR++;
		}
		else 
		{*/
		for (int i = 0; checkOR[i] != NULL; i++)
		{
			parseSemiAND(checkOR[i], argv);
		}
		for (int i = 0; argv[i] != NULL; i++)
		{
			argv1[i] = new char*[500];
			parseSpace(argv[i], argv1[i]);
		}

		//}
		for (int i = 0; argv1[i] != NULL; i++)
		{
			isExit(argv1[i][0]); 
			forkID = fork();
			//int success = 
			checkFork(forkID, argv1[i]);
			/*if (success == 0 && useOR == 1) // Exit with error
			{
				break;
			}*/
		}
			
		memset(argv1, 0, 500);
		memset(userinput, 0, 500); //Set all to 0 for fresh input
		memset(argv, 0, 500);	   //Set all to 0 for fresh args
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
