#ifndef EXECUTION_H
#define EXECUTION_H
#include <pthread.h>
#include "path_misc.h"
#define PIPE_SIZE 512
#define PIPE_NOT_ABLE_TO_OPEN_RETURN 22
#define CANT_MAKE_PTHREAD_FOR_FS_REDERECTION 100
#define EXECVE_PROBLEM 27

int runTheCommand(struct codeBreaking *cb);
#endif
