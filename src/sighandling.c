#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "../include/sighandling.h"
#include "../include/shellutils.h"
#include "../include/jobcontrol.h"

void sigchld_handler(int sig) {
		
		printf("Reached SIGCHLD handler\n");
		sigset_t mask_all, mask_prev;							// Signal masks
		sigemptyset(&mask_prev);									// mask_prev blocks nothing
		sigfillset(&mask_all);										// mask_all blocks everything
		
		pid_t reaped_pid;

		// Save errno 
		int olderrno = errno;

		// Reap all possible children 
		while ((reaped_pid = waitpid(-1, NULL, WNOHANG)) > 0) {	// NOTE: pid is a global variable
				
				printf("Reaping process %d\n", reaped_pid);

				// All signals should be blocked to prevent race conditions
				// and other nasty concurrency errors
				sigprocmask(SIG_BLOCK, &mask_all, &mask_prev);

				// Delete the job from the job list 
				deljob(reaped_pid);

				// Unblock signals 
				sigprocmask(SIG_SETMASK, &mask_prev, NULL);
		}

		pid = reaped_pid;

		// Restore errno
		errno = olderrno;
}
