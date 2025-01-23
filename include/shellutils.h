#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#ifndef SHELLUTILS_H 
#define SHELLUTILS_H

#define MAXARGS 128			// Max number of arguments allowed
#define MAXLEN 512			// Max length of command 
#define MAX_ARGLEN 64 	// Max length of each argument

int parseLine(char* cmd, char** argv, int* argc);
void eval(char* cmd, char** argv, int argc, char* wd, int* wd_end, int bg);
char* innermostDir(char* wd);

extern int curr_length;

extern sigset_t mask, prev_mask, all_mask;		// Signal masks

#endif
