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
#include <vector>

using namespace std;

bool succeed = true;
bool setExit = false;
bool skipone = 0;
int prevSym = -1;
int pid = 0;
int fd[2];

void loginInfo(char* login, char* hostname, size_t hostLength)
{
	passwd* loginDetails = getpwuid(getuid());
	if (loginDetails == NULL)
	{
		perror("getpwuid");
		return;
	}
	else
	{
		login = strcpy(login, loginDetails->pw_name);
	}
	int gethost = gethostname(hostname, hostLength);
	if (gethost == -1)
	{
		perror("Error retrieving hostname");
		exit(1);
	}
}

void spaceDelim(string& input, const string delim)
{
	int ors = input.find(delim, 0);
	string temp = "";
	unsigned index = 0;
	while (ors != -1)
	{
		temp = temp + input.substr(index, ors-index) + " " + delim + " ";
		index = ors + 2;
		if (index > input.size())
		{
			break;
		}
		ors = input.find(delim, index);
	}
	if (index < input.size())
	{
		temp = temp + input.substr(index, input.size() - index);
	}
	input = temp;
}	

/*void spaceDelimPipe(string& input, const string delim)
{
	int pip = input.find(delim, 0);
	string temp = "";
	unsigned index = 0;
	while (parse != -1)
	{
		temp = temp + input.substr(index, pip-index) + " " + delim + " ";
		index = pip + 2;
*/	
		
		

void modifyInput(string& input)
{
	spaceDelim(input, "||");
	spaceDelim(input, "&&");
}

void modifyInputPipes(string& input)
{
	spaceDelim(input, "|");
	spaceDelim(input, "<<<");
	spaceDelim(input, ">>");
}

void separate(vector <char*> semis, vector <char**>& exes)
{
	char* temp = 0;
	char** hold = 0;

	for (unsigned i = 0; i < semis.size(); i++)
	{
		hold = new char*[1024];
		memset(hold, 0, 1024);
		temp = strtok(semis.at(i), " ");
		for (int j = 0; temp != NULL; j++)
		{	
			hold[j] = temp;
			temp = strtok(NULL, " ");
		}
		exes.push_back(hold);
	}
}

void executecmd(char** toRun)
{
	int status = 0;
	succeed = true;
	if (toRun[0] && strcmp(toRun[0], "exit") == 0)
	{
		setExit = true;
		return;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		if (execvp(toRun[0], toRun) == -1)
		{
			perror("execvp");
		}
		setExit = true;
	}
	else if (pid > 0)
	{
		if (wait(&status) == -1)
		{
			perror("wait");
			exit(1);
		}
		if (WIFEXITED(status) && (WEXITSTATUS(status) != 0))
		{
			succeed = false;
		}
		pid = 0;
	}	
}
	
void executePipes(vector <char**> exes)
{
	char** toRun = new char*[1024];
	memset(toRun, 0, 1024);
	int toRunIndex = 0;
	if (toRunIndex) { };
	vector <string> allOPS;
	for(unsigned i = 0; i < exes.size(); i++)
	{
		for (int j = 0; exes.at(i)[j]; j++)
		{
			if (strcmp(exes.at(i)[j], "|") == 0)
			{
				allOPS.push_back("|");
			}
			else if (strcmp(exes.at(i)[j], ">>") == 0)
			{
				allOPS.push_back(">>");
			}
			else if (strcmp(exes.at(i)[j], ">") == 0)
			{
				allOPS.push_back(">");
			}
			else if (strcmp(exes.at(i)[j], "<<<") == 0)
			{
				allOPS.push_back("<<<");
			}
		}

		for (int j = 0; exes.at(i)[j]; j++)
		{
			if (setExit == true)
			{
				break;
			}
			else if (strcmp(exes.at(i)[j], "<") == 0)
			{
				if (exes.at(i)[j+1])
				{
					if (exes.at(i)[j+2])	
					{
						string nextOP = exes.at(i)[j+2];
					}
				}
			}
		}
	}
}

void executeLines(vector <char**> exes)
{
	char** toRun = new char*[1024];
	memset(toRun, 0, 1024);
	int toRunIndex = 0;
	for(unsigned i = 0; i < exes.size(); i++)
	{
		for(int j = 0; exes.at(i)[j]; j++)
		{
			if (setExit == true)
			{
				break;
			}
			else if (strcmp(exes.at(i)[j], "&&") == 0)
			{
				if (skipone == 0)
				{
					executecmd(toRun);
					if (!succeed)
					{
						skipone = 1;
					}
				}
				else if (skipone == 1)
				{
					if (prevSym == 1)
					{
						skipone = 0;
					}
				}
				
				prevSym = 0;
				memset(toRun, 0, 1024);
				toRunIndex = 0;
			}
			else if (strcmp(exes.at(i)[j], "||") == 0)
			{
				if (skipone == 0)
				{
					executecmd(toRun);
					if (succeed)
					{
						skipone = 1;
					}
				}
				else if (skipone == 1)
				{
					if (prevSym == 0)
					{
						skipone = 0;
					}
				}
					
				prevSym = 1;
				memset(toRun, 0, 1024);
				toRunIndex = 0;
			}
			else
			{
				if (skipone == 0)
				{
					toRun[toRunIndex] = exes.at(i)[j];
					toRunIndex++;
					if (exes.at(i)[j+1] == NULL)
					{
						executecmd(toRun);
					}
				}
			}
		}
		if (setExit == true)
		{
			break;
		}

		succeed = 1;
		skipone = 0;
		prevSym = -1;
		toRunIndex = 0;
		memset(toRun, 0, 1024);
	}
	delete [] toRun;
}
	
int main()
{
	char login[1024] = {0}, hostname[1024] = {0};
	string input = "";
	char* cmdLine = 0;
	char* temp = 0;
	vector <char**> exes;
	size_t hostLength = 1024;
	loginInfo(login, hostname, hostLength);
	cout << "Initializing RSHELL: " << endl;
	while(1)
	{
		if (strlen(login) != 0 && strlen(hostname) != 0)
		{
			printf("%s@%s", login, hostname);
		}
		cout << "$ ";
		getline(cin, input);
		input = input.substr(0, input.find('#', 0));
		if ((input.find(">", 0) != string::npos) 
			|| (input.find("<", 0) != string::npos) 
			|| ((input.find("|") != string::npos)
			&& (input.find("||") == string::npos)))
		{
			modifyInputPipes(input);
		}
			modifyInput(input);
		cmdLine = new char[input.size()+1];			
		memset(cmdLine, 0, input.size()+1);

		vector <char*> semis;
		cmdLine = strcpy(cmdLine, input.c_str());
		temp = strtok(cmdLine, ";");
		for (int i = 0; temp != NULL; i++)
		{
			semis.push_back(temp);
			temp = strtok(NULL, ";");
		}
		separate(semis, exes);

		executeLines(exes);	

		delete [] cmdLine;
		temp = 0;
		for (unsigned i = 0; i < exes.size(); i++)
		{
			delete [] exes.at(i);
		}
		exes.clear();

		if (setExit == true)
		{
			break;
		}
	}
	for (unsigned i = 0; i < exes.size(); i++)
	{
		delete [] exes.at(i);
	}
	if (pid == 0)
	{
		return 1;
	}
	return 0;
}

































