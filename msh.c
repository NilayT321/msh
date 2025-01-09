#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include "devutils.h"
#include "shellutils.h"

int curr_length;

int main() {
		
		// Working directory
		curr_length = 512;
		char* wd = calloc(512, sizeof(char));
		int wd_end = 0; // Index of final non-null character in wd

		// Starting directory is the home directory
		gotoHome(wd, &wd_end);

		// Main loop 
		while (1) {
				
				printf("msh %s > ", innermostDir(wd));

				char cmdtext[MAXLEN];
				fgets(cmdtext, MAXLEN, stdin);
				
				if (feof(stdin)) {
						break;
				}

				// The arguments array 
				char** argv = calloc(MAXARGS, sizeof(char*));
				for (int i = 0; i < MAXARGS; i++) {
						argv[i] = calloc(MAX_ARGLEN, sizeof(char));
				}

				int argc = 0;

				parseLine(cmdtext, argv, &argc);

				// Evaluate
				eval(cmdtext, argv, argc, wd, &wd_end);
				
				for (int i = 0; i < argc; i++) {
						free(argv[i]);
				}
				free(argv);
				argv = NULL;
		}
		
		free(wd);
		return 0;
}
