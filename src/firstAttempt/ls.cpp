#include <iostream>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define FLAG_a 1
#define FLAG_R 2
#define FLAG_l 4

using namespace std;

int largestSize(vector <dirent*> check)
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < check.size(); i++)
	{
		if (strlen(check.at(i)->d_name) > count)
		{
			count = strlen(check.at(i)->d_name);
		}
	}
	return count;
}

void printDIRS(vector <dirent*> toPrint, int flags, const char* dir) 
{
	/*if ((flags & FLAG_R) || (error))
	{
		cout << dir << ":" << endl;
	}*/
	if (flags & FLAG_l)
	{
		for (unsigned i = 0; i < toPrint.size(); i++)
		{
			char *path = new char[1024];
			memset(path, 0, 1024);
			strcat(path, dir);
			strcat(path, "/");
			strcat(path, toPrint.at(i)->d_name);
			struct stat *buf = new struct stat[1024];
			memset(buf, 0, 1024);
			int statCheck = stat(path, buf);
			if (statCheck == -1)
			{
				perror("stat in print");
				exit(1);
			}
			char fileType = '-';
			char uR = '-', uWR = '-', uEX = '-';
			char gR = '-', gWR = '-', gEX = '-';
			char oR = '-', oWR = '-', oEX = '-';
			struct passwd *userID = getpwuid(buf->st_uid);
			struct group *grpID = getgrgid(buf->st_gid);
			struct tm *tm = localtime(&(buf->st_mtime));
			char time[13] = {0};
			size_t max = 13;
			strftime(time, max, "%b %e %H:%M" ,tm); 
			if (S_ISDIR(buf->st_mode))
			{
				fileType = 'd';
			}
			else if (S_ISLNK(buf->st_mode))
			{
				fileType = 'l';
			}
			if (buf->st_mode & S_IRUSR)
			{
				uR = 'r';
			}
			if (buf->st_mode & S_IWUSR)
			{
				uWR = 'w';
			}
			if (buf->st_mode & S_IXUSR)
			{
				uEX = 'x';
			}
			if (buf->st_mode & S_IRGRP)
			{
				gR = 'r';
			}
			if (buf->st_mode & S_IWGRP)
			{
				gWR = 'w';
			}
			if (buf->st_mode & S_IXGRP)
			{
				gEX = 'x';
			}
			if (buf->st_mode & S_IROTH)
			{
				oR = 'r';
			}
			if (buf->st_mode & S_IWOTH)
			{
				oWR = 'w';
			}
			if (buf->st_mode & S_IXOTH)
			{
				oEX = 'x';
			}
									
			//print blocks
			printf("%c%c%c%c%c%c%c%c%c%c ", fileType, uR, uWR, uEX, gR, gWR, gEX, oR, oWR, oEX);
			printf("%d ", (int)buf->st_nlink);
			//print user
			printf("%s ", userID->pw_name);
			printf("%s ", grpID->gr_name);
			printf("%7d ", (int)buf->st_size);
			printf("%s ", time); 
			printf("%s\n", toPrint.at(i)->d_name);
			
			delete [] path;
			delete [] buf;

		}		
	}
	else 
	{
		int size = largestSize(toPrint) + 2;
		int printCount = 0;
		int numPrint = 0;
		for (unsigned int i = 0; i < toPrint.size(); i++)
		{
			/*if (printCount + size > 80 || (numPrint >= 6))
			{
				cout << endl;
				numPrint = 0;
				printCount = 0;
				i--;
			}*/
			if (printCount + (size*2) > 80 || (numPrint >= 6))
			{
				printf("%-*s\n", size, toPrint.at(i)->d_name);
				printCount = 0;
				numPrint = 0;
			}
			else if (printCount + (size*2) <= 80 && (numPrint < 6))
			{
				printf("%-*s", size, toPrint.at(i)->d_name);
				printCount += size;
				numPrint++;
			}
		}
		cout << endl;
	}
	cout << endl;
		
}

int compareVect(vector <char> left, vector <char> right)
{
	for (unsigned i = 0; i < left.size() && i < right.size(); i++)
	{
		if (left.at(i) < right.at(i))
		{
			return -1;
		}
		else if (left.at(i) > right.at(i))
		{
			return 1;
		}
	}
	if (left.size() < right.size())
	{
		return -1;
	}
	else if (left.size() > right.size())
	{
		return 1;
	}
	return 0;
}


void mergeSort(vector <dirent*>& sortThis)
{
	unsigned int size = sortThis.size();
	if (size <= 1)
	{
		return;
	}
	unsigned int midpoint = size/2;
	unsigned int i = 0, j = 0, k = 0;
	vector <dirent*> tempLeft;
	vector <dirent*> tempRight;
	for (i = 0; i < midpoint; i++)
	{
		tempLeft.push_back(sortThis.at(i));
	}
	for (j = 0; i < size; i++)
	{
		tempRight.push_back(sortThis.at(i));
		j++;
	}
	mergeSort(tempLeft);
	mergeSort(tempRight);
	sortThis.clear();
	i = 0, j = 0, k = 0;
	while ((j < tempLeft.size()) || (k < tempRight.size()))
	{
		if (j >= tempLeft.size())
		{
			sortThis.push_back(tempRight.at(k));
			k++;
		}
		else if (k >= tempRight.size())
		{
			sortThis.push_back(tempLeft.at(j));
			j++;
		}
		else
		{
			vector <char> compL;
			vector <char> compR;
			for (unsigned l = 0; l < strlen(tempLeft.at(j)->d_name); l++)
			{
				compL.push_back(toupper(tempLeft.at(j)->d_name[l]));
			}
			for (unsigned m = 0; m < strlen(tempRight.at(k)->d_name); m++)
			{
				compR.push_back(toupper(tempRight.at(k)->d_name[m]));
			}
			int result = compareVect(compL, compR);
			if (result < 0)
			{
				sortThis.push_back(tempLeft.at(j));
				j++;
			}
			else if (result > 0)
			{
				sortThis.push_back(tempRight.at(k));
				k++;
			}
			else
			{
				sortThis.push_back(tempLeft.at(j));
				sortThis.push_back(tempRight.at(k));
				i++;
				j++;
				k++;
			}
		}
		i++;	
	}
}

int checkFlags(const char **argv, int &flags, vector <const char*> &dirNames)
{
	int errorCheck = 0;
	for (int i = 1; argv[i]; i++)
	{
		if (argv[i][0] == '-')
		{
			for (int j = 1; argv[i][j]; j++)
			{
				if (argv[i][j] == 'a')
				{
					flags = flags | FLAG_a;
				}
				else if (argv[i][j] == 'l')
				{
					flags = flags | FLAG_l;
				}
				else if (argv[i][j] == 'R')
				{
					flags = flags | FLAG_R;
				}
				else
				{
					printf("ls: invalid option -- '%c'\n", argv[i][j]);
					printf("Available flags: -a -R -l\n");
					exit(1);
				}
			}
		}
		else
		{
			const char *path = argv[i];
			DIR *dirp = opendir(path);
			if ((errno & ENOENT) != 0)
			{
				printf("ls: cannot access %s: No such file or directory\n", path);
				errno = 0;
				errorCheck++;
			}	
			else if (dirp == 0)
			{
				perror("opendir in checkFlags");
				exit(1);
			}
			else 
			{
				dirNames.push_back(argv[i]);
				if (closedir(dirp) == -1)
				{
					perror("closedir in checkFlags");
					exit(1);
				}
			}
		}
	}
	if (dirNames.size() == 0 && errorCheck == 0)
	{
		dirNames.push_back(".");
	}
	return errorCheck;					
}

int main(int argc, char **argv)
{
	if (argc == 0 && errno == 0)
	{
		cerr << "Not enough arguments" << endl;
		exit(1);
	}
	vector <const char*> dirNames;
	dirNames.clear();
	int flags = 0;
	int errorCheck = checkFlags((const char**)argv, flags, dirNames);
	dirent *direntp = 0;
	vector <dirent*> allDIRS;
	allDIRS.clear();
	for (unsigned a = 0; a < dirNames.size(); a++)
	{
		DIR *dirp = opendir(dirNames.at(a));
		if (dirp == NULL)
		{
			perror("opendir");
			exit(1);
		}
		
		if (flags & FLAG_a)
		{
			for (int i = 0; (direntp = readdir(dirp)); i++)
			{
				allDIRS.push_back(direntp);
			}
		}
		else 
		{
			while ((direntp = readdir(dirp)))
			{
				if (direntp->d_name[0] != '.')
				{
					allDIRS.push_back(direntp);
				}
			}
		}
		
		if (flags & FLAG_R)
		{
		
			for (unsigned i = 0; i < allDIRS.size(); i++)
			{
				char *path = new char[1024];
				memset(path, 0, 1024);
				struct stat *buf = new struct stat[1024];
				memset(buf, 0, 1024);
				strcat(path, dirNames.at(a));
				strcat(path, "/");
				strcat(path, allDIRS.at(i)->d_name);
				int statCheck = stat(path, buf);
				if (statCheck == -1)
				{
					perror("stat lsR");
					exit(1);
				}
				char comp[] = ".";
				char comp1[] = "..";
				if (S_ISDIR(buf->st_mode) && (strcmp(allDIRS.at(i)->d_name, comp) != 0) && (strcmp(allDIRS.at(i)->d_name, comp1) != 0))
				{
					char *addDIR = new char[1024];
					memset(addDIR, 0, 1024);
					strcpy(addDIR, path);
					
					dirNames.push_back(addDIR);
				}
				delete [] path;
				delete [] buf;
			}
			
		}
				
		mergeSort(allDIRS);
		if (flags & FLAG_R || errorCheck || dirNames.size() > 1)
		{
			cout << dirNames.at(a) << ":" << endl;
		}
		printDIRS(allDIRS, flags, dirNames.at(a));

		if (closedir(dirp) == -1)
		{
			perror("closedir");
			exit(1);
		}
			
		allDIRS.clear();
	}	

	for (unsigned i = 1; i < dirNames.size()-1; i++)
	{
		delete [] dirNames.at(i);
	}

	return 0;
}
