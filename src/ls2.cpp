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
#include <algorithm>
#include <string>
#include <iomanip>

#define FLAG_a 1
#define FLAG_R 2
#define FLAG_l 4

#define GREEN		"\033[1m\033[32m"		//Exe files
#define BLUE		"\033[1m\033[34m"		//Directories
#define CYAN		"\033[1m\033[36m"		//Sym-Links
#define GRAY		"\033[37m"			//Hidden files
#define GRAYSHADE	"\033[100m"		//Hidden files w/ other color
#define END		"\033[00m"			//Stops color printing


using namespace std;

int numPaths = 0;
int flags = 0;
int multipleDirs = 0;
int failedDir = 0;

bool cmp(const string& a, const string& b)
{
	string temp1 = "";
	string temp2 = "";
	for (unsigned i = 0; i < a.size(); i++)
	{
		temp1 += toupper(a.at(i));		
	}
	for (unsigned i = 0; i < b.size(); i++)
	{
		temp2 += toupper(b.at(i));		
	}
	return (temp1 < temp2);
}
		
int maxStringSize(vector <string> a)
{
	unsigned largest = 0;
	for (unsigned i = 0; i < a.size(); i++)
	{
		if (a.at(i).size() > largest)
		{
			largest = a.at(i).size();
		}
	}
	return largest;
}

int numDigits(int value)
{
	int digits = 0;
	while (value)
	{
		value /= 10;
		digits++;
	}
	return digits;
}

vector<int> statSizes(vector <string> a, string directory)
{
	vector <int> returnMe(5);
	for (unsigned i = 0; i < a.size(); i++)
	{
		struct stat buf;
		string dir = directory + "/" + a.at(i);
		if (lstat(dir.c_str(), &buf) == -1)
		{
			perror("lstat");
			exit(1);
		}
		returnMe.at(0) += buf.st_blocks;
		if (numDigits((int)buf.st_nlink) > returnMe.at(1))
		{
			returnMe.at(1) = numDigits((int)buf.st_nlink);
		}
		struct passwd* userID = 0;
		struct group* grpID = 0;
		if ((userID = getpwuid(buf.st_uid)) == NULL)
		{
			perror("getpwuid");
			exit(1);
		}
		if ((grpID = getgrgid(buf.st_gid)) == NULL)
		{
			perror("getgrgid");
			exit(1);
		}
		if ((int)strlen(userID->pw_name) > returnMe.at(2))
		{
			returnMe.at(2) = strlen(userID->pw_name);
		}
		if ((int)strlen(grpID->gr_name) > returnMe.at(3))
		{
			returnMe.at(3) = strlen(grpID->gr_name);
		}
		if (numDigits((int)buf.st_size) > returnMe.at(4))
		{
			returnMe.at(4) = numDigits((int)buf.st_size);
		}
	}
	returnMe.at(0) /= 2;
	return returnMe;
}

int checkFlags(char** argv, vector <string>& allPaths)
{
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
					return -1;
				}
			}
		}
		else
		{
			const char* path = argv[i];
			DIR* dirp = opendir(path);
			if ((errno & ENOENT) != 0 || (errno & ENOTDIR) != 0)
			{
				printf("ls: cannot acces %s: No such file or directory\n", path);
				failedDir = 1;
				errno = 0;
			}
			else if (dirp == 0)
			{
				perror("opendir");
				errno = 0;
			}
			else
			{
				if (closedir(dirp) == -1)
				{
					perror("closedir");
					errno = 0;
				}
				string temp = path;
				allPaths.push_back(temp);
				numPaths++;
			}
		}
	}
	if (allPaths.size() == 0 && failedDir == 0)
	{
		allPaths.push_back(".");
	}
	return 0;
}

vector<int> largestInColumn(vector <string> dirNames, int columns)
{
	vector <int> returnMe(columns);
	for (unsigned i = 0; i < dirNames.size(); i++)
	{
		if ((int)dirNames.at(i).size() > returnMe.at(i%columns))
		{
			returnMe.at(i%columns) = dirNames.at(i).size();
		}
	}
	return returnMe;
}

void print(vector <string> dirNames, string directory)
{

	if (flags & FLAG_l)
	{
		vector <int> sizes;
		sizes = statSizes(dirNames, directory);	
		printf("total %d\n", sizes.at(0));
		sizes.clear();
	}	

	int largestSize = maxStringSize(dirNames);
	int columns = 79/largestSize;
	largestSize += 2;
	vector <int> columnSize;
	columnSize = largestInColumn(dirNames, columns);
	columns++;
	
	for (unsigned i = 0; i < dirNames.size(); i++)
	{
		string printColor = "";
		struct stat buf;
		string toStat = directory + "/" + dirNames.at(i);
		if (lstat(toStat.c_str(), &buf) == -1)
		{
			perror("lstat");
			return;
		}
		if (dirNames.at(i).at(0) == '.')
		{
			printColor += GRAYSHADE;
		}
		if ((S_IXUSR & buf.st_mode) || (S_IXGRP & buf.st_mode) ||
			(S_IXOTH & buf.st_mode))
		{
			printColor += GREEN;
		}
		if (S_ISDIR(buf.st_mode))
		{
			printColor += BLUE;
		}
		if (S_ISLNK(buf.st_mode))
		{
			printColor += CYAN;
		}
		if (printColor == GRAYSHADE)
		{
			printColor = GRAY;
		}
		if (flags & FLAG_l)
		{
			vector <int> sizes;
			sizes = statSizes(dirNames, directory);	
			struct passwd* userID = 0;
			struct group* grpID = 0;
			struct tm* tm = 0;
			if ((userID = getpwuid(buf.st_uid)) == NULL)
			{
				perror("getpwuid");
				exit(1);
			}
			if ((grpID = getgrgid(buf.st_gid)) == NULL)
			{
				perror("getgrgid");
				exit(1);
			}
			if ((tm = localtime(&(buf.st_mtime))) == NULL)
			{
				perror("tm");
				exit(1);
			}
			char time[13] = {0};
			size_t max = 13;
			strftime(time, max, "%b %e %H:%M" , tm);
			if (S_ISDIR(buf.st_mode)) { printf("d"); }
			else if (S_ISLNK(buf.st_mode)) { printf("l"); }
			else { printf("-"); }
			(S_IRUSR & buf.st_mode) ? printf("r") : printf("-");
			(S_IWUSR & buf.st_mode) ? printf("w") : printf("-");
			(S_IXUSR & buf.st_mode) ? printf("x") : printf("-");
			(S_IRGRP & buf.st_mode) ? printf("r") : printf("-");
			(S_IWGRP & buf.st_mode) ? printf("w") : printf("-");
			(S_IXGRP & buf.st_mode) ? printf("x") : printf("-");
			(S_IROTH & buf.st_mode) ? printf("r") : printf("-");
			(S_IWOTH & buf.st_mode) ? printf("w") : printf("-");
			(S_IXOTH & buf.st_mode) ? printf("x ") : printf("- ");
			printf("%*d ", sizes.at(1), (int)buf.st_nlink);
			printf("%*s ", sizes.at(2), userID->pw_name); 
			printf("%*s ", sizes.at(3), grpID->gr_name); 
			printf("%*d ", sizes.at(4), (int)buf.st_size);
			printf("%s ", time);
			printf("%s%s%s", printColor.c_str(), dirNames.at(i).c_str(), END);
			if (S_ISLNK(buf.st_mode))
			{
				char buf1[1024] = {0};
				if (readlink(toStat.c_str(), buf1, 1024) == -1)
				{
					perror("readlink");
					exit(1);
				}
				printf(" -> %s", buf1);
			}
			printf("\n");
		}	
		else
		{
			columns--;
			if (columns == 0)
			{
				printf("\n");
				columns = columnSize.size();
			}
			int largerSize = columnSize.at(i%columnSize.size())+2;
			printf("%s%-*s%s", printColor.c_str(), (int)dirNames.at(i).size(), dirNames.at(i).c_str(), END);
			printf("%*s", largerSize - (int)dirNames.at(i).size(), "");
		}
			
	}
	if ((flags & FLAG_l) == 0)
	{
		printf("\n");
	}
}	

int main(int argc, char** argv)
{
	if (argc == 0)
	{
		cerr << "How did you do this" << endl;
		exit(1);
	}
	
	string temp = "";
	dirent* direntp = 0;
	DIR* dirp = 0;
	struct stat buf;
	vector <string> allPaths;
	vector <string> dirNames;

	if ((checkFlags(argv, allPaths)) == -1)
	{
		return 0;
	}
	for (unsigned i = 0; i < allPaths.size(); i++)
	{
		if ((dirp = opendir(allPaths.at(i).c_str())) == 0)
		{
			perror("opendir");
			errno = 0;
			if (closedir(dirp) == -1)
			{
				perror("closedir");
			}
			continue;
		}
		while ((direntp = readdir(dirp)))
		{
			if (direntp->d_name)
			{
				temp = direntp->d_name;	
				string temp2 = allPaths.at(i) + "/" + temp;
				if (lstat(temp2.c_str(), &buf) == -1)
				{	
					if ((errno & EACCES) == 0)
					{
						perror("lstat");
						exit(1);
					}
					errno = 0;
				}
				if ((flags & FLAG_a) == 0)
				{
					if (temp.at(0) != '.')
					{
						dirNames.push_back(temp);
					}
				}
				else if (flags & FLAG_a)
				{
					dirNames.push_back(temp);
				}
				if ((flags & FLAG_R) && (S_ISDIR(buf.st_mode)))
				{
					if (temp.at(0) != '.')
					{
						allPaths.push_back(temp2);
					}
				}
			}
			temp = "";
		}
		if (errno != 0)
		{
			perror("direntp");
			errno = 0;
		}
		if (closedir(dirp) == -1)
		{
			perror("closedir");
			exit(1);
		}
		dirNames.clear();
	}
	sort(allPaths.begin(), allPaths.end(), cmp);
	for (unsigned i = 0; i < allPaths.size(); i++)
	{
		if ((dirp = opendir(allPaths.at(i).c_str())) == 0)
		{
			perror("opendir");
			errno = 0;
			break;
		}
		while ((direntp = readdir(dirp)))
		{
			if (direntp->d_name)
			{
				temp = direntp->d_name;	
				string temp2 = allPaths.at(i) + "/" + temp;
				if (lstat(temp2.c_str(), &buf) == -1)
				{	
					perror("lstat");
					if ((errno & EACCES) == 0)
					{
						exit(1);
					}
					errno = 0;
				}
				if ((flags & FLAG_a) == 0)
				{
					if (temp.at(0) != '.')
					{
						dirNames.push_back(temp);
					}
				}
				else
				{
					dirNames.push_back(temp);
				}
				/*if ((flags & FLAG_R) && (S_ISDIR(buf.st_mode)))
				{
					if (allPaths.at(i).at(0) != '.')
					{
						allPaths.push_back(temp);
					}
				}*/
			}
			temp = "";
		}
		if (errno != 0)
		{
			perror("direntp");
			errno = 0;
		}
		if (closedir(dirp) == -1)
		{
			perror("closedir");
			exit(1);
		}

		sort(dirNames.begin(), dirNames.end(), cmp);
		multipleDirs = allPaths.size() - 1;
		if (flags & FLAG_R)
		{
			multipleDirs++;
		}
		if (multipleDirs != 0)
		{
			printf("%s:\n", allPaths.at(i).c_str());
		}
		print(dirNames, allPaths.at(i));
		if (multipleDirs != 0 && ((i+1) < allPaths.size()))
		{
			cout << endl;
		}
		/*for (unsigned i = 0; i < dirNames.size(); i++)
		{
			cout << dirNames.at(i).c_str() << endl;
		}*/

		dirNames.clear();
	}
	return 0;
}




































