

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "call_cmd.h"










void call_ls()

{
    execlp("ls", "ls", "-l", (char *)NULL);
    // int error = 0;
    // error = execlp("ls", "ls", "-l", (char *)NULL);
    // perror("Execute lp fails");
    // if(error==-1){
    //     exit(EXIT_FAILURE);
    // }
    
}

void call_who()

{
    execlp("who", "who", (char *)NULL); 
}

void call_date()

{
    execlp("date", "date", (char *)NULL); 
}


