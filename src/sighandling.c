#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <errno.h>
#include <signal.h> 
#include "../include/sighandling.h"
#include "../include/jobcontrol.h"
#include "../include/shellutils.h"

/*
 * This is a wrapper function for the sigaction function 
 * used to install signal handlers 
 * @return old signal handler
 */
sighandler_t install_signal(int signo, sighandler_t handler) {
		
		struct sigaction action, old_action;

		action.sa_handler = handler;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;

		if (sigaction(signo, &action, &old_action) < 0) {
				perror("sigaction");
		} 
		
		return old_action.sa_handler;

}

/* 
 * SIGCHLD handler. It is invoked anytime a process is ended or stopped 
 * FG process ends: remove from job list 
 * BG process ends: remove from job list and print a message 
 * @return Nothing
 */
void sigchld_handler(int sig) {
		
		sigset_t all_mask, prev_mask;					// Signal masks

		sigfillset(&all_mask);								// all_mask blocks everything

		pid_t curr_pid;												// PID of process that was reaped
		int status;														// status of child

		int olderrno = errno;									// Save and restore errno


		while ((curr_pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
				
				// Block all signals 
				sigprocmask(SIG_BLOCK, &all_mask, &prev_mask);

				// Get the job of the child 
				job_t* curr_job = getJob(curr_pid);

				// Read exit status 
				if (WIFEXITED(status)) {								// child terminated
						deljob(curr_pid);
				} else if (WIFSTOPPED(status)) {				// process was stopped
						// Update job state to stopped 
						curr_job->state = JOB_STOP;

				} else if (WIFSIGNALED(status)) {
						deljob(curr_pid);
				}
				
				pid = curr_pid;												// PID is a global variable

				// Unblock signals 
				sigprocmask(SIG_SETMASK, &prev_mask, NULL);
		}
		
		errno = olderrno;										// restore errno
}

/*
 * SIGINT handler. Look for the current foreground job and terminate it 
 * By send SIGINT to its process group 
 * If there is no foreground job, this should do nothing 
 * @return Nothing
 */
void sigint_handler(int sig) {
		
		// printf("Reached SIGINT handler!\n");
		
		pid_t fgJob;												// current foreground job

		int olderrno = errno;								// save errno so we can restore it 
		
		for (int i = 0; i < MAXJOBS; i++) {
				if (jobs[i].state == JOB_FG) {
						fgJob = jobs[i].pid;
						deljob(fgJob);
						kill(-fgJob, SIGINT);
				}
		}
		
		write(1, "\n", 2);

		errno = olderrno;								// restore errno

}

/* SIGTSTP handler. Look for the current foreground job and place it 
 * in the stopped state by sending SIGTSTP to all proces in its 
 * process group. 
 * If there is no foreground job, this should do nothing 
 * @return Nothing
 */
void sigtstp_handler(int sig) {
		
		pid_t fgJob; 

		int olderrno = errno;

		for (int i = 0; i < MAXJOBS; i++) {
				if (jobs[i].state == JOB_FG) {
						fgJob = jobs[i].pid;
						jobs[i].state = JOB_STOP;
						kill(-fgJob, SIGTSTP);
				}
		}

		// Print out a message 
		
		write(1, "\n", 2);

		errno = olderrno;
}
