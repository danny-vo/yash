#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Command.h"
#include "ParseTools.h"
#include "SignalHandlers.h"

void Yash_redirect(Command* cmd) {
  /* Redirect stdin */
  if (cmd->fdTable.stdIn) {
    int ifd = open(
      cmd->fdTable.stdIn,
      O_RDONLY,
      S_IRGRP| S_IROTH | S_IRUSR | S_IWUSR
    );
    dup2(ifd, STDIN_FILENO);
    close(ifd);
  }

  /* Redirect stdout */
  if (cmd->fdTable.stdOut) {
    int ofd = open(
      cmd->fdTable.stdOut,
      O_CREAT | O_WRONLY,
      S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR
    );
    dup2(ofd, STDOUT_FILENO);
    close(ofd);
  }

  /* Redirect stderr */
  if (cmd->fdTable.stdErr) {
    int efd = open(
      cmd->fdTable.stdErr,
      O_CREAT | O_WRONLY,
      S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR
    );
    dup2(efd, STDERR_FILENO);
    close(efd);
  }

  return;
}

void Yash_executeCommand(Command* cmd) {
  pid_t pid = fork();

  // Child executes the program
  if (0 == pid) {
    Yash_redirect(cmd);
    execvp(cmd->program, Command_getArgs(cmd));
  // Error
  } else if (-1 == pid) {
    perror("fork() error\n");
  // Blocking until child finishes
  } else {
    wait((int*) NULL);
  }

  close(pid);
}

int Yash_forkPipes(Command* cmd) {
  int pipeFd[2];
  pipe(pipeFd);

  pid_t pid0 = fork();
  if (0 == pid0) {
    close(pipeFd[0]);
    dup2(pipeFd[1], STDOUT_FILENO);
    Yash_redirect(cmd->pipe[0]);
    execvp(cmd->pipe[0]->program, Command_getArgs(cmd->pipe[0]));
  } else if (pid0 < 0) {
    perror("fork() error\n");
  }
  
  pid_t pid1 = fork();
  if (0 == pid1) {
    close(pipeFd[1]);
    dup2(pipeFd[0], STDIN_FILENO);
    Yash_redirect(cmd->pipe[1]);
    execvp(cmd->pipe[1]->program, Command_getArgs(cmd->pipe[1]));
  } else if (pid1 < 0) {
    perror("fork() error\n");
  }

  close(pipeFd[0]);
  close(pipeFd[1]);
  wait((int*) NULL);
  wait((int*) NULL);

  return 0;
}

int main(int argc, char* argv[]) {
  /* Assign signal handlers */
  //signal(SIGINT, sigintHandler);
  signal(SIGTSTP, sigtstpHandler);
  signal(SIGCHLD, sigchldHandler);
  
  char* inString;
  while (inString = readline("# ")) {
    Vector* commands = Parse_commands(inString);

    while (Vector_size(commands) > 0) {
      Command* cmd = Vector_pop(commands);

      if (PIPE == cmd->type) {
        Yash_forkPipes(cmd);
      } else {
        Yash_executeCommand(cmd);
      }
      Command_destroy(cmd);
    }

    Vector_destroy(commands);
  }

  return 0;

}
