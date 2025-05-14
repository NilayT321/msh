#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include "../include/devutils.h"
#include "../include/shellutils.h"
#include "../include/dirnavigating.h"
#include "../include/jobcontrol.h"
#include "../include/sighandling.h"

int curr_length;

job_t jobs[MAXJOBS];									// Jobs array
volatile int nextJID = 1;											// Next available JID

int main() {

		// Set signal masks

		// Load signal handlers
		install_signal(SIGCHLD, sigchld_handler);
		install_signal(SIGINT, sigint_handler);
		install_signal(SIGTSTP, sigtstp_handler);

		// Initialize the list of jobs 
		initjobList();

		// Working directory
		curr_length = 512;
		char* wd = calloc(512, sizeof(char));
		int wd_end = 0; // Index of final non-null character in wd

		// Starting directory is the home directory
		gotoHome(wd, &wd_end);

		// Main loop 
		while (1) {
				
				printf("msh@%s> ", innermostDir(wd));
				
				char cmdtext[MAXLEN];
				fgets(cmdtext, MAXLEN, stdin);
				
				if (feof(stdin)) {
						printf("\n");
						break;
				}
				
				// The arguments array 
				char** argv = calloc(MAXARGS, sizeof(char*));
				for (int i = 0; i < MAXARGS; i++) {
						argv[i] = calloc(MAX_ARGLEN, sizeof(char));
				}

				int argc = 0;

				int bg = parseLine(cmdtext, argv, &argc);

				// Evaluate
				eval(cmdtext, argv, argc, wd, &wd_end, bg);

				for (int i = 0; i < MAXARGS; i++) {
						free(argv[i]);
				}
				free(argv);
				argv = NULL;
		}
		
		free(wd);
		return 0;
}
