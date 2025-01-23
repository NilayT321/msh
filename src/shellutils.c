#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <string.h>
#include <stdbool.h>
#include "../include/shellutils.h" 
#include "../include/dirnavigating.h"
#include "../include/jobcontrol.h"
 
int parseLine(char* cmd, char** argv, int* argc) {
		
		int numArgs = 0;
		int bg; 						// This is 0 if the job is run in the foreground; 1 otherwise
												// Commands that end in '&' will be run in the background.

		// Remove leading spaces 
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

		// Free blocks after numArgs 
		for (int i = numArgs + 1; i < MAXARGS; i++)
				free(argv[i]);

		// Is the last member of the list empty due to trailing whitespace? 
		// If so, remove that member too 
		if (argv[numArgs-1][0] == '\0') {
				free(argv[numArgs-1]);
				argv[numArgs-1] = NULL;
				numArgs--;
		}

		// printf("There are %d arguments\n", numArgs);
		
		// If the last member of the list is an '&', we run the job in the background 
		if (argv[numArgs-1][0] == '&' && strlen(argv[numArgs-1]) == 1) {
				free(argv[numArgs-1]);
				argv[numArgs-1] = NULL;
				numArgs--;
				bg = 1;
		} else {
				bg = 0;
		}

		*argc = numArgs;

		return bg;
}

void eval(char* cmd, char** argv, int argc, char* wd, int* wd_end, int bg) {

		// Empty line is ignored 
		if (argc == 1 && strcmp(argv[0], "") == 0)
				return;

		// Builtin commands
		if (strcmp(cmd, "exit") == 0)
				exit(0);
		
		// Change directory command
		if (strcmp(argv[0], "cd") == 0) {
				cd(argv, argc, wd, wd_end);
				return;
		}

		// jobs command 
		if (strcmp(argv[0], "jobs") == 0) {
				printJobs();
				return;
		}

		// Block SIGCHLD before forking to ensure that the jobs are added correctly
		pid_t child = fork();
		int status; 
		
		if (child < 0) {
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
				
				if (bg == 0) {
						addjob(child, JOB_FG, cmd); 			// Foreground job
				} else {
						addjob(child, JOB_BG, cmd);			// Background job 
				}

				// If foregound job, then wait for child 
				if (bg == 0) {
						wait(&status);
				}
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
