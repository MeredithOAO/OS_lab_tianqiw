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

// The <unistd.h> header is your gateway to the OS's process management facilities.
#include <unistd.h>



#include "parse.h"

static void print_cmd(Command *cmd);
static void print_pgm(Pgm *p);
static int handle_cmd(Command *cmd_get);
void stripwhite(char *);
void run_cmd(Pgm *pgm_now);



int finish_flag = 0;



int main(void)
{
  Command cmd;
  char *line;

  while (!finish_flag) 
  {    
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


    while (pgm_now->next != NULL){
          pipe_counts++;
          pgm_now = pgm_now->next;
    }


    if (pipe_counts==0){
    // Fork a new process
    int pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    } 
    
    if (pid == 0){
        
        run_cmd(pgm_now);
 
    } else {
        
        if (cmd_get->background != 1) {
            wait(NULL);
        }
    }
        //print_cmd(cmd_get);
        return 0;
    }
    else{
//fuck pipeline!!!!!
    }




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