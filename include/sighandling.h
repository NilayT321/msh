#ifndef SIGHANDLING_H
#define SIGHANDLING_H 

// Signal handlers
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);

#endif
