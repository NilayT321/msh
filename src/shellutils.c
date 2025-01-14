#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <string.h>
#include <stdbool.h>
#include "../include/shellutils.h" 
#include "../include/dirnavigating.h"

void parseLine(char* cmd, char** argv, int* argc) {
		
		int numArgs = 0;
		
		// Remove trailing spaces 
		while (*cmd == ' ')
				cmd++;

		while (cmd != NULL) {

				char* space = strchr(cmd, ' ');
				
				if (space == NULL) {
						// End of string
						strncpy(argv[numArgs], cmd, strlen(cmd));
						argv[numArgs][strlen(cmd)-1] = '\0';
						break;
				} 

				int spaceIndex = space - cmd; 
				strncpy(argv[numArgs], cmd, spaceIndex);
				argv[numArgs][spaceIndex] = '\0';
				
				numArgs++;
				
				cmd = space + 1;

				// Remove any extra spaces 
				while (*cmd == ' ')
						cmd++;
		}
		
		numArgs++;
		free(argv[numArgs]);
		argv[numArgs] = NULL;
		*argc = numArgs;

		// Free blocks after numArgs 
		for (int i = numArgs + 1; i < MAXARGS; i++)
				free(argv[i]);
}

void eval(char* cmd, char** argv, int argc, char* wd, int* wd_end) {
		
		// Empty line is ignored 
		if (argc == 1 && strcmp(argv[0], "") == 0)
				return;

		// Builtin commands
		if (strcmp(cmd, "quit") == 0)
				exit(0);
		
		// Change directory command
		if (strcmp(argv[0], "cd") == 0) {
				cd(argv, argc, wd, wd_end);
				return;
		}

		// Create a new process 
		pid_t child;
		int status; 

		if ((child = fork()) < 0) {
				perror("fork error\n");
				exit(EXIT_FAILURE);
		} else if (child == 0) {
				// Change the process to a new process group 
				// That way, we can handle child processes accordingly
				setpgid(0,0);

				if (execvp(argv[0], argv) < 0) {
						perror("msh");
				}
				
		} else {
				waitpid(child, &status, 0);
		}
}


char* innermostDir(char* wd) {
		
		if (strcmp(wd, getenv("HOME")) == 0) {
				return "~";	
		} else {
				// Find the last occurence of '/' in wd 
				char* last = strrchr(wd, '/');

				return last + 1;
		}
}


