#include <stdio.h>
#include "../include/devutils.h"


void printArray(char** arr, int size) {
		for (int i = 0; i < size; i++) {
				printf("\"%s\" ", arr[i]);
		}
		printf("\n");
}

void printJob(job_t job) {
		printf("[%d], %d, %d, %s\n", job.jid, job.pid, job.state, job.cmdtext);
}

void printJobArray(job_t* jobs, int size) {
		for (int i = 0; i < size; i++) 
				printJob(jobs[i]);

		printf("\n");
}
