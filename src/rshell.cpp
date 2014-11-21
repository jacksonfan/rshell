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
#include <fcntl.h>


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
	else if (strcmp(op, " <<< ") == 0)
	{
		addSpace = strcat(addSpace, toParse+i+3);
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
				i++;
			}
			else
			{
				addSpaces(toParse, addSpace, " > ", i);
			}
			i++;
		}
		else if (toParse[i] == '<')
		{
			if (toParse[i+1] && toParse[i+1] == '<')
			{
				if (toParse[i+2] == '<')
				{
					addSpaces(toParse, addSpace, " <<< ", i);
					i += 2;
				}
			}
			else
			{
				addSpaces(toParse, addSpace, " < ", i);
			}
			i++;
		}
	}
}

void parseSpace(char* toParse, char**& argv)
{
	char* temp = strtok(toParse, " ");
	argv[0] = temp;
	for (int i = 1; temp != NULL; i++)
	{
		temp = strtok(NULL, " ");	
		argv[i] = temp;
	}
}

int getSize(char** argv)
{
	int i = 0;
	for (i = 0; argv[i]; i++);
	return i;
}

void executeLine(char** argv)
{
	int fd[2];
	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(1);
	}
	char* flags = 0;
	char* prevFlag = 0;
	for (int i = 0; argv[i]; i++)
	{
		char** temp = new char*[getSize(argv)+1];
		for (int p = 0; p < getSize(argv)+1; p++)
		{
			temp[p] = 0;
		}
		char* holder = 0;
		for (int j = 0; (argv[i]) && (argv[i][0] != '>')
					&& (argv[i][0] != '<')
					&& (argv[i][0] != '|'); j++)
		{
			holder = new char[strlen(argv[i])+1];
			memset(holder, 0, strlen(argv[i])+1);
			temp[j] = strcpy(holder, argv[i]);
			i++;
		}
		char** toRun = new char*[getSize(temp)+3];
		for (int p = 0; p < getSize(temp)+3; p++)
		{
			toRun[p] = 0;
		}
		for (int j = 0; j < getSize(temp)+1; j++)
		{
			toRun[j] = temp[j];
		}
		if (argv[i] == NULL)
		{
			i--;
		}
		else if (argv[i] != NULL)
		{
			flags = argv[i];
		}
		
		int pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(1);
		}
		else if (pid == 0) //CHILD
		{
			if (prevFlag == NULL)
			{
				if (close(fd[0]) == -1)
				{
					perror("close");
					exit(1);
				}
				if ((flags != NULL) && 
					(strcmp(flags, "|") == 0 || 
					strcmp(flags, ">") == 0 || 
					strcmp(flags, ">>") == 0))
				{	
					if (dup2(fd[1], STDOUT_FILENO) == -1)
					{
						perror("dup2");
						exit(1);
					}
					if (close(fd[1]) == -1)
					{
						perror("close");
						exit(1);
					}
					if (execvp(toRun[0], toRun) == -1)
					{
						perror("execvp");
						exit(1);
					}
				}
				else if (flags != NULL && strcmp(flags,"<") == 0)
				{
					toRun[getSize(toRun)] = argv[i+1];
					i++;
					if (dup2(fd[1], STDOUT_FILENO) == -1)
					{
						perror("dup2");
						exit(1);
					}

					if (close(fd[1]) == -1)
					{
						perror("close");
						exit(1);
					}

					if (-1 == execvp(toRun[0], toRun))
					{
						perror("execvp");
						exit(1);
					}
				}
				else if (flags == NULL)
				{
					if (close(fd[1] == -1))
					{
						perror("close");
						exit(1);
					}
					if (-1 == execvp(toRun[0], toRun))
					{
						perror("execvp");
						exit(1);
					}
				}

			}
			else if (strcmp(prevFlag, "|") == 0)
			{
				if (dup2(fd[0], STDIN_FILENO) == -1)
				{
					perror("dup2");
					exit(1);
				}
				if (close(fd[0]) == -1)
				{
					perror("close");
				}
				if ((flags != NULL) && (strcmp(flags, "|") == 0))
				{
					if (dup2(fd[1], STDOUT_FILENO) == -1)
					{
						perror("dup2");
						exit(1);
					}
					if (close(fd[1]) == -1)
					{
						perror("close");
						exit(1);
					}
				}
				if (execvp(toRun[0], toRun) == -1)
				{
					perror("execvp");
					exit(1);
				}
			}
			else if (strcmp(prevFlag, ">") == 0 ||
				strcmp(prevFlag, ">>") == 0)
			{
				char buf[BUFSIZ] = {0};
				int fileD = 0;
				if (strcmp(prevFlag, ">") == 0)
				{
					fileD = open(toRun[0],
						O_CREAT | O_WRONLY, 0644);
				}
				else if (strcmp(prevFlag, ">>") == 0)
				{
					fileD = open(toRun[0], O_CREAT |
						O_WRONLY | O_APPEND, 0644);
				}
				if (fileD == -1)
				{
					perror("open");
					exit(1);
				}
				int readCheck = 0;
				while ((readCheck = read(fd[0], &buf, BUFSIZ)))
				{
					if (readCheck == -1)
					{
						perror("read");
						exit(1);
					}
					if (write(fileD, buf, BUFSIZ) == -1)
					{
						perror("write");
						exit(1);
					}
				}
				if (close(fileD) == -1)
				{
					perror("close");
					exit(1);
				}
			}	
			else if (strcmp(prevFlag, "<") == 0)
			{
				if (close(fd[1]) == -1)
				{
					perror("close");
					exit(1);
				}
				if (flags == NULL)
				{
					int readCheck = 0;
					char buf[BUFSIZ] = {0};
					while ((readCheck = 
						read(fd[0], &buf, BUFSIZ)))
					{
						if (readCheck == -1)
						{
							perror("read");
							exit(1);
						}
						if (write(1, buf, readCheck)
							== -1)
						{
							perror("write");
							exit(1);
						}
					}
				}
			}
			exit(1);
					
		}
		else if (pid > 0)
		{
			if (-1 == wait(NULL))
			{
				perror("wait");
				exit(1);
			}

		}
		prevFlag = flags;
		flags = NULL;
			
		delete [] toRun;
		delete [] temp;
	}
	if ((close(fd[0]) == -1))
	{
		perror("close3");
		exit(1);
	}
	if ((close(fd[1]) == -1))
	{
		perror("close2");
		exit(1);
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
		if (input != "")
		{
			strcpy(toParse, input.c_str());
			parseComments(toParse); // Parse for #s only
			//parseInpOP(toParse); //Parse for < only
			parseIO(toParse);
			char** argv = new char*[strlen(toParse)+1];
			memset(argv, 0, strlen(toParse)+1);
			parseSpace(toParse, argv);
			executeLine(argv);





			delete [] argv;
		}
		if (strcmp(toParse, "exit") == 0)
		{
			break;
		}

		delete [] toParse;
		toParse = 0;
	}
	

	return 0;
}
