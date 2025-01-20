#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <signal.h>
#include <errno.h>
#include "../include/sighandling.h"

volatile pid_t foreground_pid;

/* 
 * Used to catch SIGINT's (Ctrl+C) from the terminal 
 * @return Nothing
 */
void sigint_handler(int sig) {
		
		int olderrno = errno;
		printf("Handler is running. Sending a signal to %d\n", foreground_pid);
		if (foreground_pid == 0) {

		} else {
				if (kill(-1 * foreground_pid, SIGINT) < 0) {
						perror("msh");
				} 
				
		}

		errno = olderrno;
}
