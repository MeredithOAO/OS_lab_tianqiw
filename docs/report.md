---
Author:
- "Group 10: Tianqi Wen, Aysha Siddika, Ateeq Ur Rehman"
Date: 2024-09-24
Title: Operating System Lab 1 Report
---

# Introduction

A shell is a program that provides an interface a user to interact with
an operating system. Shell interprets human-readable commands which
passes to kernel. the operating system then executes them according to
what a user enters. if the command is executed successfully, the shell
displays the result to the user. A simple command shell works in an
infinite loop that reads users inputs, then it is the Linux that handle
the execution part. The assigned lab, shell application, performs the
following key tasks successfully.

In this lab, we will implement our own shell that can interpret and
execute user commands, as well as complete related functionalities like
pipes. Additionally, while ensuring these functionalities, we need to
avoid issues such as zombie processes and other related problems. This
report will introduce the process of how we implemented the shell and
analyze and discuss the problems encountered along the way.

# Work Flow

Based on the provided files, we first read all the *README* files and,
after understanding the basic code structure, we decided to use the
*execvp()* system call to implement basic commands based on the contents
of *pgmlist*.

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
file_des\[i\]\[0\] is the input file descriptor of the current command,
and *file_des\[i\]\[1\]* is the output file descriptor. Next, we loop to
create the required number of pipes, assigning the write and read ends
of each pipe to the file_des array to connect adjacent commands. We then
loop and use *fork()* to create a child process for each command, locate
the current command, and handle redirection. Finally, we close
unnecessary file descriptors and execute the command. In the parent
process, after creating all child processes, we close all pipe file
descriptors because the parent process does not need to use them.

Aside from that, we were left with only the last test not passing which
is about foreground and background jobs. We used top to monitor
processes for debugging. Finally, we removed unnecessary wait calls and
modified the code to ensure that each wait corresponds to the correct
PID. We also enabled and disabled the default Ctrl+C signal handling at
the appropriate places. We ran the shell and manually entered commands,
which satisfactorily met the requirements. Ultimately, we fulfilled all
the requirements and passed all the test cases in the automated test
suite.

# Discussion

## Challenges Encounter

The most difficult bug we fixed is about CTRL-C handling with concurrent
foreground and background processes. When we failed the CTRL-C test, the
automated testing provided a very clear reason for the failure: after
pressing CTRL-C, the remaining number of processes was not equal to one.
We used top to monitor the running status of the sleep command, and we
found that when we ran sleep 60 & and sleep 60 simultaneously, pressing
CTRL-C would kill both processes. This quickly made us realize that the
background process needed to ignore the CTRL-C signal. So, after
determining the background process, we used the signal() function to
catch and handle the SIGINT signal and ignored CTRL-C in the background.

## Improve

# Conclusion

To sum up, this lab gave us the chance to better comprehend the criteria
specified in the project specifications. We were able to effectively
implement every feature, addressing them in a sensible sequence that
made testing and development go more smoothly. Every specification had
its own set of difficulties, from figuring out intricate operations to
making sure that they worked in different situations. Our ability to
solve problems has improved along with my technical capabilities after
overcoming these challenges.

Though I think there is room for development, such as better examples
and more tools for debugging, I found the offered lab materials to be
generally successful throughout the process. The automated tests were
especially helpful since they made it possible for us to swiftly find
and fix problems and make sure that my implementation complied with the
necessary requirements. But I did observe that
