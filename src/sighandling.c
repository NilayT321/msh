#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <signal.h>
#include "../include/sighandling.h"

extern volatile sig_atomic_t sig_int_flag;


