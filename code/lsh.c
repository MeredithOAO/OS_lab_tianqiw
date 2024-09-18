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

#include "call_cmd.h"

#include "parse.h"

static void print_cmd(Command *cmd);
static void print_pgm(Pgm *p);
static int execute_cmd(Command *);
void stripwhite(char *);

int main(void)
{
  for (;;)
  {
    char *line;
    line = readline("> ");

    // Remove leading and trailing whitespace from the line
    stripwhite(line);

    // If stripped line not blank
    if (*line)
    {
      add_history(line);

      Command cmd;
      if (parse(line, &cmd) == 1)
      {
        // Just prints cmd
       execute_cmd(&cmd);

        // print_cmd(&cmd);


      }
      else
      {
        printf("Parse ERROR\n");
      }
    }

    // Clear memory
    free(line);
  }

  return 0;
}


static int execute_cmd(Command *cmd_get)

{
    int sleep_time = 0;
    Pgm *pgm = cmd_get->pgm;
    char **pl = pgm->pgmlist;
    
   

    char *cmd_part = pl[0];

    //char *arg_part = pl[1];
    char *arg_get = pl;
    if (!strcmp(cmd_part, "exit")) {
            printf("Exit!.\n");
            exit(1);  // exit
        }


    // Fork a new process
    pid_t pid = fork();
    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    } else if (pid == 0){

        if(!strcmp(cmd_part, "ls"))
        {        
        execvp(cmd_part,arg_get);
        //call_ls(args[0]);
        //printf("%s\n",*args[0]);
        //call_ls(arg_part);
        }
        // if(!strcmp(cmd_part, "who"))
        // {call_who();}
        // if(!strcmp(cmd_part, "date"))
        // {call_date();}
        // if(!strcmp(cmd_part, "cd"))
        // {
        // call_cd(arg_part);
        // }

        
    } else {
        // Parent process: wait for the child to complete
        int status;
        if (!cmd_get->background) {
            // Only wait if the command is not set to run in the background
            waitpid(pid, &status, 0);
        }
    }
        print_cmd(cmd_get);



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


// char cmd_part[10];
    // char arg_part[10];
    // int i = 0;
    // int arg_index = 0;

    // // while(*pl[0] != ' ' && *pl[i] != '\0') 
    // // {
    // // *cmd_part[i] = *pl[i];
    // // i++;
    // // }

    // while (pl[0][i] != ' ' && pl[0][i] != '\0') {
    //     cmd_part[i] = pl[0][i];
    //     i++;
    // }

    // cmd_part[i] = '\0';

    // if (pl[0][i]== ' '){
      
      
    //   for (; ; )
    //   {
    //     i++;
    //     if (pl[0][i] == '\0'){
    //       break;
    //     }else{
    //     arg_part[arg_index] = pl[0][i];
    //     arg_index++;}

    //   }
    // arg_part[arg_index+1] = '\0';   

    // }


        // const char *split_flag = " ";
     
    // char *split_cmds[10]; 
    // int count = 0;

    // char *split_cmd = strtok(*pl, split_flag);
    // while (split_cmd != NULL) {
    //     split_cmds[count++] = split_cmd;
    //     split_cmd = strtok(NULL, split_flag);
    // }

    // char *cmd_part = split_cmds[0];
    // //char *arg_part = split_cmds[2];