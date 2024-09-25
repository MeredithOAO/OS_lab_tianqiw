---
Author:
- "Group 10: Tianqi Wen, Aysha Siddika, Ateeq Ur Rehman"
Date: 2024-09-25
Title: Operating System Lab 1 Report
---

# Introduction

In this lab, we will implement our own shell that can interpret and
execute user commands, as well as complete related functionalities like
pipes. Additionally, while ensuring these functionalities, we need to
avoid issues such as zombie processes and other related problems. This
report will introduce the process of how we implemented the shell and
analyze and discuss the problems encountered along the way.

# Work Flow

Based on the provided files, we began by thoroughly reading all the
*README* documents to gain a comprehensive understanding of the existing
codebase and its structure. This initial step was crucial in
familiarizing ourselves with the components we would be working with and
in planning our implementation strategy. After grasping the basic code
structure, we decided to use the *execvp()* system call to implement the
execution of basic commands. We utilized the contents of the *pgmlist*
array, which holds the parsed user commands, to pass arguments to
*execvp()*.

For handling Ctrl-C, we called *signal(SIGINT, sigint_handler_parent)*
at the beginning of the main function to catch and handle the *SIGINT*
signal. This ensures that when the process receives a *SIGINT* signal,
it invokes *sigint_handler_parent* to close the foreground process
without shutting down the shell itself.

At this point, we discovered that the *cd* command could not be
implemented using *execvp()* because it starts a new process to execute
commands, so *cd* command would not change the shell's own directory.
Therefore, we check if *pgmlist\[0\]* is the *cd* command and use
*chdir()* to change the working directory of the current process.
Similarly, for the exit command, we check if *pgmlist\[0\]* is exit to
perform the exit action.

We then created a *redirections()* function that, based on *rstdout* and
*rstdin* in the *cmd* structure, redirects the specified files to the
standard output and standard input of the current process, respectively.
This ensures that subsequent I/O(Input/Output) operations use the
correct file streams. By calling the *redirections()* function before
executing commands, we implemented the feature of enabling standard
input and output redirection to files.

For implementing pipes, we need to make the output of the previous
command serve as the input for the next command to handle multiple user
commands. First, based on the next field in the *pgm* structure, we
calculate the number of pipes. We then decided to create a
two-dimensional array *fd_pipe_creat\[pipe_counts\]\[2\]* to store the
input and output file descriptors for each command, where
*file_des\[i\]\[0\]* is the input file descriptor of the current
command, and *file_des\[i\]\[1\]* is the output file descriptor. Next,
we loop to create the required number of pipes, assigning the write and
read ends of each pipe to the *file_des* array to connect adjacent
commands. We then loop and use *fork()* to create a child process for
each command, locate the current command, and handle redirection.
Finally, we close unnecessary file descriptors and execute the command.
In the parent process, after creating all child processes, we close all
pipe file descriptors because the parent process does not need to use
them.

Aside from that, we were left with only the last test not passing which
is about foreground and background jobs. We used top to monitor
processes for debugging. Finally, we removed unnecessary wait calls and
modified the code to ensure that each wait corresponds to the correct
*PID*. We also enabled and disabled the default Ctrl-C signal handling
at the appropriate places. We ran the shell and manually entered
commands, which satisfactorily met the requirements. Ultimately, we
fulfilled all the requirements and passed all the test cases in the
automated test suite.

# Discussion

## Challenges Encountered

The most difficult bug we fixed is about CTRL-C handling with concurrent
foreground and background processes. When we failed the CTRL-C test, the
automated testing provided a very clear reason for the failure: after
pressing CTRL-C, the remaining number of processes was not equal to one.

To diagnose the problem, we used the *top* command to monitor the
real-time status of running processes, specifically focusing on the
sleep command. We executed sleep 60 & to start a background sleep
process and sleep 60 to start a foreground sleep process simultaneously.
Upon pressing CTRL-C, we observed that both the background and
foreground sleep processes were terminated.

This observation led us to realize that our shell was not
differentiating between foreground and background processes concerning
signal handling. The background process was inadvertently receiving and
responding to the SIGINT signal, which it should ignore to continue
running in the background.

To resolve the issue, we adjusted our code to identify whether a process
was running in the foreground or background. After determining the
process type, we used the *signal()* function to catch and handle the
SIGINT signal appropriately. For background processes, we set the signal
handler to ignore the SIGINT signal by using *signal(SIGINT, SIG_IGN)*.
This ensured that background processes would not respond to CTRL-C
inputs from the user. For foreground processes, we maintained the
default behavior, allowing them to be terminated by CTRL-C as expected.

Regarding the implementation of the pipeline functionality in our shell,
we initially decided to adopt a recursive approach. The recursive method
seemed elegant and theoretically suitable because pipelines can
naturally be represented as recursive structures, with each command
feeding its output into the next command. However, as we proceeded, the
bugs that emerged were exceedingly difficult to troubleshoot, making it
challenging to pinpoint the root causes of the issues.

One of the main difficulties stemmed from the inherent complexity of
recursion, especially in the context of system programming, where
resources like file descriptors and process IDs must be meticulously
managed. Each recursive call adds a new layer to the call stack, which
complicates the control flow and makes it more difficult to track the
program's state at any given point. Managing resources across multiple
recursive calls increased the risk of resource leaks and unintended
behaviors, such as open file descriptors not being properly closed.

Finally, we decided to create a two-dimensional array and replace the
recursive method with a for loop. By calling pipe() for each pair of
adjacent commands, we set up the necessary pipes to handle inter-process
communication. We used fork() to create child processes for each command
and dup2() to redirect the standard input and output, connecting the
processes via pipes. This iterative approach allowed us to manage all
the pipes and processes systematically. It enabled us to set breakpoints
within the loop and observe the state of variables at each iteration,
which greatly facilitated the debugging process. We could monitor the
creation and closure of file descriptors, ensure that child processes
were correctly managed, and verify that data flowed properly through the
pipeline.

## Improve

We realize that our teamwork needs significant strengthening. Initially,
we decided that each of us would independently complete the entire
experiment before coming together for a discussion. However, during the
discussion phase, we discovered that the varying progress levels among
team members hindered effective communication. Some members were ahead
while others were behind, which made it challenging to engage in
productive discussions and reach collective decisions.

In light of this experience, we believe that adopting a more
collaborative approach would be beneficial. Next time, we plan to work
more closely together and synchronize our progress to ensure that
everyone can actively participate in discussions. By coordinating our
efforts and maintaining the same pace, we aim to enhance our teamwork,
improve communication, and ensure that all team members contribute
meaningfully to the project.

# Conclusion

In this project, we successfully implemented our own shell capable of
interpreting and executing user commands, including advanced
functionalities such as piping, redirection, and signal handling. We
began by thoroughly analyzing the provided files and understanding the
existing code structure, which allowed us to effectively plan our
implementation strategy.

Moreover, the provided automated testing was highly effective, allowing
us to efficiently identify potential issues after completing each
portion of functionality. Although further analysis was required to
pinpoint the root causes of the detected problems, being able to run
automated tests during the coding process enabled us to promptly
discover and address problems in the code as we developed it.

Overall, this project was a valuable learning experience that deepened
our understanding of process management, inter-process communication,
and signal handling in Unix-like operating systems. It also highlighted
the importance of effective teamwork and communication. Moving forward,
we plan to adopt a more collaborative approach to ensure synchronized
progress and active participation from all team members in future
projects.
