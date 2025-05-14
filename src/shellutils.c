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

volatile sig_atomic_t pid;					// Global PID for the foreground job
 

int parseLine(char* cmd, char** argv, int* argc) {
		
		int numArgs = 0;
		int bg; 						// This is 0 if the job is run in the foreground; 1 for background; -1 on error
												// Commands that end in '&' will be run in the background.
		
		// Ignore empty strings 
		if (cmd[0] == '\0')
				return -1;

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

		sigset_t prev_mask, sigchld_mask, mask_all;						// signal masks, prev_mask should block nothing
		
		sigfillset(&mask_all);											// mask_all blocks everything
		sigemptyset(&sigchld_mask);
		sigaddset(&sigchld_mask, SIGCHLD);					// sigchld_mask blocks SIGCHLD only

		// Empty line is ignored 
		if ((argc == 1 && strcmp(argv[0], "") == 0) || bg == -1)
				return;
		
		// Builtin commands
		if (builtins(cmd, argv, argc, wd, wd_end) == 1)
				return;

		if (bg == 0) {
				
				// Block SIGCHLD to prevent race conditions 
				sigprocmask(SIG_BLOCK, &sigchld_mask, &prev_mask);
				pid_t child = fork();

				if (child < 0) {
						perror("fork");
						exit(1);
				} else if (child == 0) {
						// Unblock all signals before executing
						sigprocmask(SIG_SETMASK, &prev_mask, NULL);

						// Put the child in its own process group 
						// That way, it doesn't interfere with our shell when we do signal stuff
						setpgid(0, 0);
						
						if (execvp(argv[0], argv) < 0) {
								printf("%s: %s\n", argv[0], strerror(errno));
								return;
						}
				} else {
						// PARENT
						// Add to job list
						// First block all signals 
						sigprocmask(SIG_BLOCK, &mask_all, NULL);
						addjob(child, JOB_FG, cmd);

						// Wait for a signal 
						pid = 0;
						while(pid == 0) 
								sigsuspend(&prev_mask);

						// Unblock signals 
						sigprocmask(SIG_SETMASK, &prev_mask, NULL);

				}
		} else {
				
				// Background job 
				// Block SIGCHLD again 
				pid_t child = fork();

				if (child < 0) {
						perror("fork");
						exit(1);
				} else if (child == 0) {
						// Unblock signals 
						sigprocmask(SIG_SETMASK, &prev_mask, NULL);

						setpgid(0, 0); 			

						if (execvp(argv[0], argv) < 0) {
								printf("%s: %s\n", argv[0], strerror(errno));
								return;
						}
				} else {
						// PARENT
						// Add to job list
						// Block signals to prevent race conditions
						sigprocmask(SIG_BLOCK, &mask_all, NULL);
						addjob(child, JOB_BG, cmd);
						
						// Unblock signals 
						sigprocmask(SIG_SETMASK, &prev_mask, NULL);

						// Print a message 
						job_t* currBGJob = getJob(child);
						printf("[%d] %d\n", currBGJob->jid, currBGJob->pid);

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

		// fg 
		if (strcmp(argv[0], "fg") == 0) {
				
				// Get the second argument
				char* secondArg = argv[1];
				int which;

				if (secondArg != NULL && secondArg[0] == '%') {
						
						which = 0;

						// The JID starts after the '%' sign 
						char* jid_str = secondArg + 1;
						int id = atoi(jid_str);

						fgJob(id, which);
				} else {
						which = 1; 
						int id = atoi(secondArg);

						fgJob(id, which);
				}
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
