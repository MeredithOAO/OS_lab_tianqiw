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
void redirections(Command *cmd_now);
static void sigint_handler_parent(int sig);
static void sigint_handler_foreground(int sig);



int finish_flag = 0;
pid_t foreground_pid = -1;

int main(void)
{
  Command cmd;
  char *line;

  signal(SIGINT, sigint_handler_parent);
  signal(SIGCHLD, SIG_IGN);
  foreground_pid = getpid();

  while (1)
  {

    line = readline("> ");
    if (line == NULL)
      {  
        signal(SIGQUIT, SIG_IGN);
        kill(0, SIGQUIT);
        exit(0);
      }

      stripwhite(line);

      if (*line)
      {
        add_history(line);
        parse(line, &cmd);

        if (cmd.background)
        {
          int pid = fork();
          if (pid == 0)
          {

            signal(SIGINT, SIG_IGN); //disable ctrl+c
            handle_cmd(&cmd);
            exit(0);
          }
        }else{handle_cmd(&cmd);}



        
      }

    if (line)
    {
      free(line);
    }
  }

  return 0;
}

static void sigint_handler_parent(int sig)
{
  (void)sig; // Avoid warning
  
  if (foreground_pid != -1)
  {
    kill(foreground_pid, SIGINT);
    foreground_pid = -1; // undefined behaviour?
  }
}

static void sigint_handler_foreground(int sig)
{
  (void)sig; // Avoid warning
  exit(0);
}


static int handle_cmd(Command *cmd_get)

{
  // int sleep_time = 0;
  int chld_pid;
  int pipe_counts = 0;

  Pgm *pgm_now = cmd_get->pgm;
  char **pgmlist = pgm_now->pgmlist;

  char *cmd_part = pgmlist[0];
  char *arg_first = pgmlist[1]; // get path for cd

  if (!strcmp(cmd_part, "exit"))
  {
    exit(0);
  }

  // handle cd
  if (!strcmp(cmd_part, "cd"))
  {

    if (chdir(arg_first) < 0)
    {
      printf("failed to change directory to %s \n", arg_first);
    }
    return 0;
  }

  if (cmd_get->background == 1)
  { 
    // if in background !!!!!

  }
  else
  {
    
  }

  while (pgm_now->next != NULL)
  { //!!!!!!
    pipe_counts++;
    pgm_now = pgm_now->next;
  }

  int file_des[pipe_counts][2];
  // int pid = fork();
  if (pipe_counts == 0)
  {   
    // Fork a new process
    int pid = fork();

    if (pid < 0)
    {
      // Fork failed
      perror("fork failed");
      exit(1);
    }

    if (pid == 0)
    {
          if (cmd_get->background != 1)
    {
      signal(SIGINT, sigint_handler_foreground);
    }
    //   signal(SIGINT, SIG_DFL); // recovery signal
      redirections(cmd_get);
      run_cmd(pgm_now);
    }
    else
    {

    if (cmd_get->background) {
    foreground_pid = foreground_pid;
        } else {
    foreground_pid = pid;
}
    int status;
    waitpid(pid, &status, 0);



    }
    // print_cmd(cmd_get);
     return 0;
  }
  else
  { 
    Pgm *pgm_now_dup = cmd_get->pgm;
    int pipe_index = 0;
    int fd_pipe_creat[pipe_counts][2];
    file_des[0][0] = STDIN_FILENO;            // First command reads from standard input
    file_des[pipe_counts][1] = STDOUT_FILENO; // Last command writes to standard output

    // Create pipes and assign fd
    for (int i = 0; i < pipe_counts; i++)
    {
      if (pipe(fd_pipe_creat[i]) == -1)
      {
        perror("Pipe failed");
        return -1;
      }
      file_des[i][1] = fd_pipe_creat[i][1];     // Write end for the current command
      file_des[i + 1][0] = fd_pipe_creat[i][0]; // Read end for the next command
    }

    for (pipe_index; pipe_index <= pipe_counts; pipe_index++)
    { // Let main process fork a child for each command

      int pid = fork();
      if (pid == -1)
      {
        perror("Fork failed");
        exit(1);
      }

      if (pid == 0)
      {
        signal(SIGINT, SIG_DFL); // enable ctrl_c

        for (int i = pipe_counts; i > pipe_index; i--)
        {
          pgm_now_dup = pgm_now_dup->next;
        }

        redirections(cmd_get);

        if (file_des[pipe_index][1] != STDOUT_FILENO)
        {
          if (dup2(file_des[pipe_index][1], STDOUT_FILENO) == -1)
          {
            perror("dup2 stdout failed");
            exit(1);
          }
        }
        if (file_des[pipe_index][0] != STDIN_FILENO)
        {
          if (dup2(file_des[pipe_index][0], STDIN_FILENO) == -1)
          {
            perror("dup2 stdin failed");
            exit(1);
          }
        }

        for (int i = 0; i <= pipe_counts; i++)
        {
          if (file_des[i][0] != STDIN_FILENO)
          {
            close(file_des[i][0]);
          }
          if (file_des[i][1] != STDOUT_FILENO)
          {
            close(file_des[i][1]);
          }
        }

        run_cmd(pgm_now_dup);
      }
      else
      {   

      }
    }
  }
  // Close parents pipes
  if (pipe_counts > 0)
  {
    for (int i = 0; i < pipe_counts; i++)
    {
      close(file_des[i][0]);
      close(file_des[i][1]);
    }
  }

  // Wait for all children if in foreground
  if (!(cmd_get->background))
  { // If not in background
    for (int i = 0; i <= pipe_counts; i++)
    {
      int status;
      wait(&status);
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
  char **pgmlist = pgm_now->pgmlist;
  char *arg_all = pgmlist;

  execvp(pgmlist[0], arg_all);
  perror("execvp failed");
  exit(1);
}

void redirections(Command *cmd_now)
{
  if (cmd_now->rstdout != NULL)
  {
    int fd_out = creat(cmd_now->rstdout, 0644); // 0644 arg for normal permission settings
    if (fd_out == -1)
    {
      perror("can not open output file");
      exit(1);
    }
    if (dup2(fd_out, STDOUT_FILENO) == -1)
    {
      perror("stdout fail");
      exit(1);
    }
  }
  if (cmd_now->rstdin != NULL)
  {
    int fd_in = open(cmd_now->rstdin, 0); // 0 arg for read only
    if (fd_in == -1)
    {
      perror("can not open input file");
      exit(1);
    }
    if (dup2(fd_in, STDIN_FILENO) == -1)
    {
      perror("stdin fail");
      exit(1);
    }
  }
}
