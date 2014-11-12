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

void printDIRS(vector <dirent*> toPrint) 
{
	int size = largestSize(toPrint) + 2;
	int printCount = 0;
	for (unsigned int i = 0; i < toPrint.size(); i++)
	{
		if (printCount + (size*2) <= 80)
		{
			printf("%-*s", size, toPrint.at(i)->d_name);
		}
		else if (printCount + (size*2) > 80)
		{
			printf("%-*s\n", size, toPrint.at(i)->d_name);
		}
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

void checkFlags(const char **argv, int &flags, vector <const char*> &dirNames)
{
	int errorCheck = 0;
	for (int i = 1; argv[i]; i++)
	{
		if (argv[i][0] == '-')
		{
			for (int j = 0; argv[i][j]; j++)
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
					
}

int main(int argc, char **argv)
{
	if (argc == 0 && errno == 0)
	{
		cerr << "Not enough arguments" << endl;
		exit(1);
	}
	vector <const char*> dirNames;
	int flags = 0;
	checkFlags((const char**)argv, flags, dirNames);
	dirent *direntp = 0;
	vector <dirent*> allDIRS;
	for (unsigned a = 0; a < dirNames.size(); a++)
	{
		DIR *dirp = opendir(dirNames.at(a));
		if (dirp == NULL)
		{
			perror("opendir");
			exit(1);
		}
		if (flags == 0)
		{
			while ((direntp = readdir(dirp)))
			{
				if (direntp->d_name[0] != '.')
				{
					allDIRS.push_back(direntp);
				}
			}
		}
		else if (flags & FLAG_a)
		{
			for (int i = 0; (direntp = readdir(dirp)); i++)
			{
				allDIRS.push_back(direntp);
			}
		}
		
		mergeSort(allDIRS);
		printDIRS(allDIRS);

		if (closedir(dirp) == -1)
		{
			perror("closedir");
			exit(1);
		}
			
		allDIRS.clear();
	}	




	return 0;
}
