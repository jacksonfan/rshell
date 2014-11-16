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
	char* temp = strtok(parseThis, "#");
	strcpy(parseThis, temp);
}

char* parseOperators(char* toParse)
{
	char* temp = NULL;
	char* toRun = new char[strlen(toParse)];
	memset(toParse, 0, sizeof(toParse)*(strlen(toParse)));
	if ((strchr(toParse, '<') = temp) != NULL)
	{
		size_t cutOff = strlen(toParse) - strlen(temp);
		toRun = strcat(toRun, toParse, cutOff*sizeof(toParse));
		toRun = strcat(toRun, temp+1);
	}
	else
	{
		delete [] toRun;
		return toParse;
	}
	return toRun;
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
		memset(toParse, 0, sizeof(toParse)*(input.length()+1)); 
		strcpy(toParse, input.c_str());
		parseComments(toParse);
		//Put logic parsing here. To do after hw2. Strncpy
		char* toRun = parseOperators(toParse);		






	}
	


	return 0;
}
