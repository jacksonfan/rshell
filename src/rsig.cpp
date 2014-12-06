#include <iostream>
#include <signal.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

void sigC(int sig)
{
	if (signal(sig, SIG_DFL) == SIG_ERR)
	{
		perror("signal DFL");
		exit(1);
	}
	if (kill(getpid(), sig) == -1)
	{
		perror("kill");
		exit(1);
	}
}

void getLogin(char* login, char* hostName, size_t hostLength)
{
	passwd* loginDetails = 0;
	if ((loginDetails = getpwuid(getuid())) == NULL)
	{
		perror("getpwuid");
	}
	else 
	{
		strcpy(login, loginDetails->pw_name);
		login = loginDetails->pw_name;
	}
	int getHost = gethostname(hostName, hostLength);
	if (getHost == -1)
	{
		perror("gethostname");
	}
}

void parseInput(char** argv, const string input)
{
	memset(argv, 0, 1024);
	char* cinput = new char[input.size()+1];
	memset(cinput, 0, input.size()+1);
	cinput = strcpy(cinput, input.c_str());
	char* temp = 0;
	temp = strtok(cinput, " ");
	for (int i = 0; temp != NULL; i++)
	{
		argv[i] = new char[strlen(temp)+1];
		memset(argv[i], 0, strlen(temp)+1);
		argv[i] = strcpy(argv[i], temp);
		temp = strtok(NULL, " ");
	}
	delete [] cinput;
}

void forkAndExec(char** argv, const string input)
{
	char* command = new char[1024];
	memset(command, 0, 1024);
	parseInput(argv, input);
	int pid = fork();
	if (pid == -1)
	{	
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		if (signal(SIGINT, sigC) == SIG_ERR)
		{
			perror("signal");
			exit(1);
		}
		char* path = getenv("PATH");
		char* temp = strtok(path, ":");
		string theCom = argv[0];
		for (int i = 0; temp != NULL; i++)
		{	
			command = strcpy(command, temp);
			command = strcat(command, "/");
			command = strcat(command, theCom.c_str());
			argv[0] = command;
			execv(argv[0], argv);
			temp = strtok(NULL, ":");
			memset(command, 0, 1024);
		}		
		if (errno != 0)
		{
			perror("execv");
			exit(1);
		}
		//execvp(argv[0], argv);
	}
	else if (pid > 0)
	{
		if (wait(NULL) == -1)
		{
			perror("wait");
			exit(1);
		}
		delete [] command;
	}
}

void runCD(string input)
{
	int i = input.find(" ", 0);
	string dirs = "";
	string hello = "";
	if (i == -1 || (dirs = input.substr(i+1, input.size())) == "")
	{
		if(chdir(getenv("HOME")) == -1)
		{
			perror("chdir");
		}
	}
	else
	{
		char* path = new char[1024];
		memset(path, 0, 1024);
		char* cwdir = new char[1024];
		memset(cwdir, 0, 1024);
		cwdir = getcwd(cwdir, 1024);
		if (cwdir == NULL)
		{
			perror("cwdir");
			exit(1);
		}
		else
		{
			path = strcpy(path, cwdir);
			path = strcat(path, "/");
			path = strcat(path, dirs.c_str());
			if (chdir(path) == -1)
			{
				perror("chdir");
			}
		}
		delete [] cwdir;
		delete [] path;
	}
}

int main()
{
	string input = "";
	char* currDIR = new char[1024];
	char** argv = new char*[1024];
	memset(currDIR, 0, 1024);
	memset(argv, 0, 1024);
	if (signal(SIGINT, SIG_IGN) == SIG_ERR)
	{
		perror("signal");
		exit(1);
	}
	cout << "Entering shell: " << endl;
	while(1)
	{
		currDIR = getcwd(currDIR, 1024);
		cout << currDIR << "$ ";
		getline(cin, input);
		if(input == "exit")
		{
			break;
		}
		if(input.substr(0, 3) == "cd " || input == "cd")
		{
			runCD(input);
		}
		else
		{
			forkAndExec(argv, input);
		}

		for (int i = 0; argv[i]; i++)
		{
			delete [] argv[i];
		}
		memset(currDIR, 0, 1024);
		memset(argv, 0, 1024);
	}
	cout << "Exiting shell" << endl;

	delete [] currDIR;
	delete [] argv;

	return 0;
}
