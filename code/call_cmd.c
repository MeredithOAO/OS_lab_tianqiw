

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "call_cmd.h"










void call_ls(char *arg_part)

{
    //char *pathget = arg_part;
    //printf("%s\n",arg_part);
    execlp("ls", "ls", arg_part, (char *)NULL);
    //execlp("ls", "ls", arg_part, (char *)NULL);
    // int error = 0;
    // error = execlp("ls", "ls", "-l", (char *)NULL);
    // perror("Execute lp fails");
    // if(error==-1){
    //     exit(EXIT_FAILURE);
    // }
    
}


void call_cd(char *arg_part)

{
    //if(arg_part == 1)
    //perror("Execute lp fails");
    //char *arg_get = *arg_part;
    //execlp("cd", "cd", arg_get, (char *)NULL);
    chdir(arg_part);     
}

void call_who()

{
    execlp("who", "who", (char *)NULL); 
}

void call_date()

{
    execlp("date", "date", (char *)NULL); 
}


