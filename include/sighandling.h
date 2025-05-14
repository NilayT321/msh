#include <unistd.h> 
#include <signal.h> 

#ifndef SIGHANDLING_H
#define SIGHANDLING_H

// Signal handler type
typedef void (*sighandler_t)(int);

// A wrapper for sigaction
sighandler_t install_signal(int signo, sighandler_t handler);

// Signal handlers 
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);

#endif
