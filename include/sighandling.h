#include <signal.h>
#include <unistd.h> 
#include <sys/types.h>

#ifndef SIGHANDLING_H
#define SIGHANDLING_H

// Signal handler type
typedef void (*sighandler_t)(int);

// We need this to keep track of which process is running in our "foreground"
extern volatile pid_t foreground_pid;

void sigint_handler(int sig);

#endif
