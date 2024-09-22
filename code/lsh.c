/*
 * Main source code file for lsh shell program
 *
 * You are free to add functions to this file.
 * If you want to add functions in a separate file(s)
 * you will need to modify the CMakeLists.txt to compile
 * your additional file(s).
 *
 * Add appropriate comments in your code to make it
 * easier for us while grading your assignment.
 *
 * Using assert statements in your code is a great way to catch errors early and make debugging easier.
 * Think of them as mini self-checks that ensure your program behaves as expected.
 * By setting up these guardrails, you're creating a more robust and maintainable solution.
 * So go ahead, sprinkle some asserts in your code; they're your friends in disguise!
 *
 * All the best!
 */
#include <assert.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
// The <unistd.h> header is your gateway to the OS's process management facilities.
#include <unistd.h>



#include "parse.h"

static void print_cmd(Command *cmd);
static void print_pgm(Pgm *p);
static int handle_cmd(Command *cmd_get);
void stripwhite(char *);
void run_cmd(Pgm *pgm_now);
void handle_sigint(int sig);
void Redirections (Command *cmd_now);
void sigchldHandler(int sig);


int finish_flag = 0;
pid_t mainpid;


int main(void)
{
  Command cmd;
  char *line;
  signal(SIGINT, SIG_IGN);
  mainpid = getpid();
  signal(SIGCHLD,handle_sigint);
  //signal(SIGCHLD, sigchldHandler);
  while (!finish_flag) 
  {    
    signal(SIGCHLD, sigchldHandler);
    line = readline("> ");
    
    if (!line)
    {
      finish_flag=1;
    }
    else
    {
      stripwhite(line);
          
      if(*line) 
        {
          add_history(line);
          parse(line, &cmd);
            
          handle_cmd(&cmd);
        }
    }
    
    if(line) 
    {
      free(line);
    }
  }

  return 0;
}

void sigchldHandler(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) { //Wait for any child without blocking
        //printf("PID of terminated child %d\n", pid);
    }
    return;
  
}

void handle_sigint(int sig){

if (getppid() == mainpid)
{
    // exit(0);
    kill(mainpid, SIGINT);
}else{
  printf("\nShell: No foreground process. Press Ctrl-D to exit.\n");
}





}

static int handle_cmd(Command *cmd_get)

{


    //int sleep_time = 0;
    int chld_pid;
    int pipe_counts = 0;


    Pgm *pgm_now = cmd_get->pgm;
    char **pgmlist = pgm_now->pgmlist;
    
    char *cmd_part = pgmlist[0];
    char *arg_first = pgmlist[1];//get path for cd

    if (!strcmp(cmd_part, "exit")) {
    exit(0); 
    }

    //handle cd  
    if (!strcmp(cmd_part, "cd")) {
       
        if (chdir(arg_first) < 0) {
            printf("failed to change directory to %s \n", arg_first);
        }
        return 0;
    }

    if (cmd_get->background == 1) { //if in background !!!!!
        signal(SIGCHLD, sigchldHandler);
    } else {
        //printf("Sighandler set to not in background\n");
        //signal(SIGCHLD, SIG_DFL); //Set to default and make parent wait instead, change SIGINT 
    }


    while (pgm_now->next != NULL){  //!!!!!!
          pipe_counts++;
          pgm_now = pgm_now->next;
    }

    int fds[pipe_counts+1][2]; //fd for read and write for all future processes, starting from the first process !!!!!!
    memset( fds, 0, (pipe_counts+1)*2*sizeof(int) );


    if (pipe_counts==0){
    // Fork a new process
    int pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    } 
    
    if (pid == 0){
        signal(SIGINT, SIG_DFL);//recovery signal
        Redirections (cmd_get);
        run_cmd(pgm_now);
 
    } else {
        mainpid = pid;
        if (cmd_get->background != 1) {
            wait(NULL);
        }
    }
        //print_cmd(cmd_get);
        //return 0;
    }
    else{
//

//Create pipes and execute processes
      
        fds[0][0] = STDIN_FILENO; //first one should read from standard input
        fds[pipe_counts][1] = STDOUT_FILENO; //Last one should write to standard output

        for (int i = 0; i<pipe_counts; i++) { //Create all the necessary pipes
            int fd[2];
            if (pipe(fd) == -1) {
                fprintf(stderr, "Pipe failed");
                return -1;
            }
            fds[i][1] = fd[1]; //write to this pipe
            fds[i+1][0] = fd[0]; //read from this pipe
        }

        for (int i = 0; i <= pipe_counts; i++) { //Let main process fork a child for each command
            int pid = fork();
            if (pid == -1) {
                perror("Fork failed");
                exit(1);
            }
            
            if(pid == 0) {
                signal(SIGINT, SIG_DFL);

                if (cmd_get->background == 1) { //if in background
                    setpgid(0,0); //Change
                } 
                
                Pgm *current_pgm = cmd_get->pgm;
                for (int j = pipe_counts; (j-i)>0; j-- ) { //Make current_pgm the the command to be executed
                    current_pgm = current_pgm->next;
                }

                Redirections(cmd_get);

                if (fds[i][1] != STDOUT_FILENO ) {
                    close(STDOUT_FILENO);  //closing stdout
                    dup2(fds[i][1], STDOUT_FILENO); //replacing stdout with pipe write
                    close(fds[i][1]);
                }
                if (fds[i][0] != STDIN_FILENO) {
                    close(STDIN_FILENO);  //closing stdin
                    dup2(fds[i][0], STDIN_FILENO); //replacing stdin with pipe read
                    close(fds[i][0]);
                }

                //close pipes not used
                for (int k = 0; k<i; k++) { //for lower
                    if (fds[k][0] != STDIN_FILENO) {
                        close(fds[k][0]);
                    }
                    close(fds[k][1]);
                }
                for (int l = i+1; l<=pipe_counts; l++) { //for higher
                    close(fds[l][0]);
                    if (fds[l][1] != STDOUT_FILENO) {
                        close(fds[l][1]);
                    }
                }
                run_cmd(current_pgm);
            } else { //if parent

            }
        }
    }
    //Close parents pipes
    if (pipe_counts>0) {

        for (int m = 1; m<pipe_counts; m++) {
            close(fds[m][0]);
            close(fds[m][1]);
        }
        close(fds[0][1]); //close write of first pipe
        close(fds[pipe_counts][0]); //close read from last pipe
    }

    //Wait for all children if in foreground
    if (!(cmd_get->background)) { //If not in background
        for (int i = 0; i<= pipe_counts; i++) { //Wait for all children
            wait(NULL);
        }
    



    }


return 0;
}



/*
 * Print a Command structure as returned by parse on stdout.
 *
 * Helper function, no need to change. Might be useful to study as inspiration.
 */
static void print_cmd(Command *cmd_list)
{
  printf("------------------------------\n");
  printf("Parse OK\n");
  printf("stdin:      %s\n", cmd_list->rstdin ? cmd_list->rstdin : "<none>");
  printf("stdout:     %s\n", cmd_list->rstdout ? cmd_list->rstdout : "<none>");
  printf("background: %s\n", cmd_list->background ? "true" : "false");
  printf("Pgms:\n");
  print_pgm(cmd_list->pgm);
  printf("------------------------------\n");
}

/* Print a (linked) list of Pgm:s.
 *
 * Helper function, no need to change. Might be useful to study as inpsiration.
 */
static void print_pgm(Pgm *p)
{
  if (p == NULL)
  {
    return;
  }
  else
  {
    char **pl = p->pgmlist;

    /* The list is in reversed order so print
     * it reversed to get right
     */
    print_pgm(p->next);
    printf("            * [ ");
    while (*pl)
    {
      printf("%s ", *pl++);
    }
    printf("]\n");
  }
}


/* Strip whitespace from the start and end of a string.
 *
 * Helper function, no need to change.
 */
void stripwhite(char *string)
{
  size_t i = 0;

  while (isspace(string[i]))
  {
    i++;
  }

  if (i)
  {
    memmove(string, string + i, strlen(string + i) + 1);
  }

  i = strlen(string) - 1;
  while (i > 0 && isspace(string[i]))
  {
    i--;
  }

  string[++i] = '\0';
}

void run_cmd(Pgm *pgm_now)
{
    //int nmb_args = 0;
    int arg_counts = 0;
    char **pgmlist = pgm_now->pgmlist;
    char *arg_all = pgmlist;

    while (pgmlist[arg_counts] != NULL) {
        arg_counts++;
    }// get numbers of args

    execvp(pgmlist[0], arg_all);
        perror("execvp failed");
        exit(1);


}


void Redirections (Command *cmd_now) {       //!!!!!!!!!
  if (cmd_now->rstdout!=NULL) { //check if we have to redirect in an file
    int fdOut=creat(cmd_now->rstdout, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
    dup2(fdOut, 1); //the "1" is the f. descriptor for the output stream of the program
    close(fdOut);
  }
  if (cmd_now->rstdin!=NULL) { //check if we have to take an input from a file
    int fdIn=open(cmd_now->rstdin, 0);
    dup2(fdIn, 0); // "0" is the f. descriptor for input stream
    close(fdIn);
  }
}