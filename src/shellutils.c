#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include "../include/shellutils.h" 

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
				if (execvp(argv[0], argv) < 0) {
						perror("msh");
				}
				
		} else {
				waitpid(child, &status, 0);
		}
}

void cd(char** argv, int argc, char* wd, int* wd_end) {
		
		// If ., we stay at the same directory and do nothing 
		if (strcmp(argv[1], ".") == 0) 
				return;

		// If blank or tilde, then we go to the home directory 
		if (argc == 1 || strcmp(argv[1], "~") == 0) {
				gotoHome(wd, wd_end);
				return;
		} else if (argv[1][0] == '/') {
				// If it begins with a '/', we take this to be an absolute path 
				if (chdir(argv[1]) < 0) {
						perror("cd");
				}

				strncpy(wd, argv[1], curr_length);
				*wd_end = strlen(argv[1]) - 1;
				return;
		} else if (argv[1][0] == '~') {
				// Tilde expansion 
				// First go to home directory 
				char* home = getenv("HOME");
				if (chdir(home) < 0) {
						perror("cd");
						return;
				}

				// Get contents after the tilde. Following the first slash 
				char* slash = strchr(argv[1], '/');

				// Go to that directory 
				if (chdir(slash+1) < 0) {
						perror("cd");
						return;
				}

				// Tilde expansion is treated as an absolute path. So we change the entire wd
				strncpy(wd, home, curr_length);

				// Concatenate the rest of the string to the end 
				strncat(wd, slash, strlen(slash) + 1);
				return;
				
		} else {
				// Relative paths. These are recognized by chdir 
				if (chdir(argv[1]) < 0) {
						perror("cd");
						return;
				}

		}

		// Update the working directory
		updateWD(argv, wd, wd_end);

}

void gotoHome(char* wd, int* wd_end) {
		
		// Get the HOME envrionment variable
		char* home = getenv("HOME");
		if (home == NULL) {
				perror("getenv");
				exit(EXIT_FAILURE);
		}
		
		if (chdir(home) < 0) {
				perror("chdir");
				return;
		}

		strncpy(wd, home, MAXLEN);
		*wd_end = strlen(home) - 1;
}

void toParentDirectory(char** argv, char* wd, int* wd_end) {
		
		// Find the last occurence of the '/' character 
		char* end = strrchr(wd, '/');

		// The end position becomes a null terminator 
		*end = '\0';

		// Everything after becomes a null 
		memset(end + 1, 0, curr_length - *wd_end);
		*wd_end = end - wd;
}

void resizeWD(char* wd, int* wd_end) {
		
		wd = realloc(wd, 2 * curr_length);
		curr_length *= 2;
}

void updateWD(char** argv, char* wd, int* wd_end) {
		
		char* path = argv[1];
		char* ptr;
		
		ptr = strtok(path, "/");

		// If this is null, then there is only one navigation in path. 
		// We parse that appropriately
		if (ptr == NULL) {
				if (strcmp(path, "..") == 0) {
						toParentDirectory(argv, wd, wd_end);
						return;
				}
				
				// Be wary of the size of WD. 
				// If the load factor would get above 0.75, we resize the array as needed 
				if (strlen(path) + strlen(wd) >= 0.75 * curr_length) {
						resizeWD(wd, wd_end);
				}

				// Else we just concatenate
				strcat(wd, "/");
				strncat(wd, path, strlen(path));
				*wd_end = strlen(wd) - 1;
		}

		while (ptr != NULL) {
				// printf("%s\n", ptr);

				if (strcmp(ptr, "..") == 0) {
						toParentDirectory(argv, wd, wd_end);
						return;
				}

				// Be wary of the size of WD. 
				// If the load factor would get full, we resize the array as needed 
				if (strlen(path) + strlen(wd) >= 0.75 * curr_length) {
						resizeWD(wd, wd_end);
				}

				// Else we just concatenate 
				strcat(wd, "/");
				strncat(wd, ptr, strlen(ptr));
				*wd_end = strlen(wd) - 1;

				ptr = strtok(NULL, "/");
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
