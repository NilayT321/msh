#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "../include/shellutils.h" 
#include "../include/dirnavigating.h"
#include "../include/jobcontrol.h"
#include "../include/sighandling.h"

pid_t pid;					// Global PID for the foreground job
 
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

		sigset_t prev_mask, sigchld_mask, all_mask;

		sigemptyset(&prev_mask); 										// prev_mask blocks no signals
		sigemptyset(&sigchld_mask);
		sigaddset(&sigchld_mask, SIGCHLD);					// sigchld_mask blocks SIGCHLD only
		sigfillset(&all_mask);											// all_mask blocks everything

		// Empty line is ignored 
		if (argc == 1 && strcmp(argv[0], "") == 0)
				return;

		// Builtin commands 
		int is_builtin = builtins(cmd, argv, argc, wd, wd_end);
		if (is_builtin == 1)
				return;
		
		// Foregorund job steps: 
		// 1) Fork the process. 
		// 2) Parent should add the child to the job list 
		// 		NOTE: we need to block SIGCHLD for this 
		// 		otherwise, there is a potential race condition 
		// 3) After adding to job list, parent should wait for the child to finish

		// Block SIGCHLD to prevent race conditions 
		sigprocmask(SIG_BLOCK, &sigchld_mask, &prev_mask);

		if (bg == 0) {
				
				pid_t child = fork();

				if (child < 0) {
						perror("fork");
						exit(1);
				} else if (child == 0) {
						// CHILD: must do these things 
						// Child inherits blocked signals from parent
						// Unblock all signals before executing
						sigprocmask(SIG_SETMASK, &prev_mask, NULL);

						if (execvp(argv[0], argv) < 0) {
								printf("%s: %s\n", argv[0], strerror(errno));
								return;
						}
				} else {
						// PARENT
						// Add to job list
						addjob(child, JOB_BG, cmd);

						// wait(NULL);
						pid = 0;
						while (pid == 0) {
								sigsuspend(&prev_mask);
						}

						// Unblock SIGCHLD
						sigprocmask(SIG_SETMASK, &prev_mask, NULL);
				}
		} else {
				
				// Background job 
				// Block SIGCHLD again 
				sigprocmask(SIG_BLOCK, &sigchld_mask, &prev_mask);
				pid_t child = fork();

				if (child < 0) {
						perror("fork");
						exit(1);
				} else if (child == 0) {

				} else {
						// PARENT
				}
		}
}

/*
 * Scans the command to see if it is a builtin 
 * @return 1 if builtin was executed; 0 otherwise 
 */
int builtins(char* cmd, char** argv, int argc, char* wd, int* wd_end) {
		
		// exit 
		if (strcmp(cmd, "exit") == 0) {
				exit(0);
		}

		// cd
		if (strcmp(argv[0], "cd") == 0) {
				cd(argv, argc, wd, wd_end);
				return 1;
		}

		// jobs 
		if (strcmp(argv[0], "jobs") == 0) {
				printJobs();
				return 1;
		}

		return 0;
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
