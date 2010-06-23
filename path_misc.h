#ifndef PATH_MISC_H
#define PATH_MISC_H
#include "linked_list.h"

#define BUF_LEN 512

struct codeBreaking{
    int fs; // the file descriptor to be read from
    list * argv; //arguments as a list
    char * buf;
    int len;
    int index;
    char * command;
    // here will be the executable/function information
};

int fileIsExec(const char * name);
char* pathToExecutable(char* name, char* path);
void  nextChar(struct codeBreaking * code, char * c);
void setCommand(struct codeBreaking * code, char ** argument);
int nextArgument(char ** arg, struct codeBreaking * code);
void cutCommand(struct codeBreaking * code );
void printCommand (struct codeBreaking * code);
int runTheCommand(struct codeBreaking *cb);
#endif
