#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h>
#include "../include/jobcontrol.h"

/*
 * Adds a job with the specified parameters to the job list 
 * @return Nothing
 */
void addjob(pid_t pid,int state, char* cmdtext) {
		
		int i; 
		
		// PID of 0 means that this entry is unoccupied
		for (i = 0; i < MAXJOBS; i++) {
				if (jobs[i].pid == 0)
						break;
		}

		jobs[i].pid = pid; 
		jobs[i].jid = nextJID;
		jobs[i].state = state;
		strncpy(jobs[i].cmdtext, cmdtext, MAXLEN);

		nextJID++;
}

/*
 * Prints a list of jobs. 
 * Used when the user types in the builtin "jobs" command
 * @return Nothing 
 */
void printJobs() {
		
		for (int i = 0; i < MAXJOBS; i++) {
				
				if (jobs[i].pid != 0) {
						job_t currJob = jobs[i];
						char* currJobState;

						switch (currJob.state) {
								case JOB_BG:
								case JOB_FG:
										currJobState = "Running";
										break;

								case JOB_STOP:
										currJobState = "Stopped";
										break;

								default:
										currJobState = "Undefined";
						}

						printf("[%d] %d  %s  %s\n", currJob.jid, currJob.pid, currJobState, currJob.cmdtext);
				}
		}
}

/*
 * Clears the requested job i.e. sets its parameters to default values
 * @return Nothing
 */
void clearjob(job_t* job) {
		job->pid = 0;
		job->jid = 0;
		job->state = JOB_UNDEF;
		job->cmdtext[0] = '\0';
}

/* 
 * Deletes the job with specific PID from the job list 
 * @return Nothing 
 */
void deljob(pid_t pid) {
		for (int i = 0; i < MAXJOBS; i++) {
				if (jobs[i].pid == pid) {
						clearjob(&jobs[i]);
				}
		}
}

/* 
 * Initializes the job list 
 * @return Nothing
 */
void initjobList() {
		
		for (int i = 0; i < MAXJOBS; i++) 
				clearjob(&jobs[i]);
}
