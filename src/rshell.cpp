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

void loginInfo(char* login, char* hostname, size_t hostLength)
{
	passwd* loginDetails = getpwuid(getuid());
	if (loginDetails == NULL)
	{
		perror("getpwuid");
	}
	else
	{
		strcpy(login, loginDetails->pw_name);
	}
	int gethost = gethostname(hostname, hostLength);
	if (gethost == -1)
	{
		perror("gethostname");
		exit(1);
	}
	if (login == NULL)
	{
		perror("Failed to retrieve login ID");
		exit(1);
	}
}

void displayNames(char* login, char* hostname)
{
	printf("%s@%s$ ", login, hostname);
}

void parseComments(char* parseThis)
{
	if (parseThis != NULL)
	{
		char* temp = strtok(parseThis, "#");
		if (temp != NULL)
		{
			char* temp1 = new char[strlen(temp) + 1];
			memset(temp1, 0, strlen(temp) + 1);
			strcpy(temp1, temp);
			strcpy(parseThis, temp1);
			delete [] temp1;
		}
	}
}

void parseInpOP(char* toParse)
{
	char* temp = NULL;
	while ((temp = strchr(toParse, '<')) != NULL)
	{
		char* toRun = new char[strlen(toParse)+1];
		memset(toRun, 0, strlen(toParse)+1);
		size_t cutOff = strlen(toParse) - strlen(temp);
		toRun = strncat(toRun, toParse, cutOff);
		toRun = strcat(toRun, " ");
		toRun = strcat(toRun, temp+1);
		strcpy(toParse, toRun);
		delete [] toRun;
	}
}

void addSpaces(char*& toParse, char*& addSpace, const char* op, int i)
{
	addSpace = new char[strlen(toParse) + 3];
	memset(addSpace, 0, strlen(toParse)+3);
	addSpace = strncat(addSpace, toParse, i);
	addSpace = strcat(addSpace, op);
	if (strcmp(op, " >> ") == 0)
	{
		addSpace = strcat(addSpace, toParse+i+2);
	}
	else
	{
		addSpace = strcat(addSpace, toParse+i+1); 
	}
	delete [] toParse;
	toParse = new char[strlen(addSpace)+1];
	memset(toParse, 0, strlen(addSpace)+1);
	strcpy(toParse, addSpace);
	delete [] addSpace; 
	addSpace = 0;
}

void parseIO(char*& toParse)
{
	char* addSpace = NULL;
	for (int i = 0; toParse[i]; i++)
	{
		if (toParse[i] == '|')
		{
			addSpaces(toParse, addSpace, " | ", i);
			i++;
		}
		else if (toParse[i] == '>')
		{
			if (toParse[i+1] == '>')
			{
				addSpaces(toParse, addSpace, " >> ", i);
			}
			else
			{
				addSpaces(toParse, addSpace, " > ", i);
			}
			i++;
		}
	}
}
int main()
{
	char login[256] = {'\0'};
	char hostname[256] = {'\0'};
	loginInfo(login, hostname, sizeof(hostname));
	cout << "Initializing RSHELL" << endl;

	while(true)
	{
		displayNames(login, hostname);
		string input = "";
		getline(cin, input);
		char* toParse = new char[input.length()+1];
		memset(toParse, 0, input.length()+1);
		int hello = 0;
		if (input != "")
		{
			strcpy(toParse, input.c_str());
			parseComments(toParse); // Parse for #s only
			parseInpOP(toParse); //Parse for < only
			parseIO(toParse);
		}
		hello++;
		if (strcmp(toParse, "exit") == 0)
		{
			break;
		}


		delete [] toParse;
		toParse = 0;
	}
	

	return 0;
}
