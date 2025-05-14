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
int builtins(char* cmd, char** argv, int argc, char* wd, int* wd_end);
char* innermostDir(char* wd);

extern int curr_length;

extern volatile sig_atomic_t pid; 				// Foreground job PID; use when waiting for a child process to complete

#endif
