#include <unistd.h> 
#include <sys/types.h> 
#include "../include/shellutils.h"

#ifndef JOBCONTROL_H
#define JOBCONTROL_H 

#define MAXJOBS 512 				// Maximum number of jobs allowed

// Job states 
#define JOB_UNDEF 0					// Undefined job state
#define JOB_FG 1 						// Foreground job state 
#define JOB_BG 2 						// Background job state 
#define JOB_STOP 3					// Stopped job state

// Job struct 
typedef struct {
		pid_t pid;							// PID of the process
		int jid; 								// Job ID. Starts from 1, 2, ...
		int state;							// State of the job
		char cmdtext[MAXLEN];		// Command which started the job
} job_t;

extern job_t jobs[MAXJOBS];
extern volatile int nextJID;					// Next available JID

// Modify the jobs array 
void addjob(pid_t pid, int state, char* cmdtext);
void printJobs();
void clearjob(job_t* job);
void deljob(pid_t pid);
void initjobList();

// Search through jobs array
job_t* getJob(pid_t pid);
job_t* getJobJID(int jid);

// Change job state
void bgJob(int id, int which);
void fgJob(int id, int which);

#endif
