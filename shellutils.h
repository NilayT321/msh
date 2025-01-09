#ifndef SHELLUTILS_H 

#define MAXARGS 128			// Max number of arguments allowed
#define MAXLEN 512			// Max length of command 
#define MAX_ARGLEN 64 	// Max length of each argument

void parseLine(char* cmd, char** argv, int* argc);
void eval(char* cmd, char** argv, int argc, char* wd, int* wd_end);
void gotoHome(char* wd, int* wd_end);
void cd(char** argv, int argc, char* wd, int* wd_end);
void toParentDirectory(char** argv, char* wd, int* wd_end);
void resizeWD(char* wd, int* wd_end);
void updateWD(char** argv, char* wd, int* wd_end);
char* innermostDir(char* wd);

#endif
