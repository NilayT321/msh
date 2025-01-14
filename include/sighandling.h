#ifndef SIGHANDLING_H

// Signal handler type
typedef void (*sighandler_t)(int);

void sigint_handler(int sig);

#endif
