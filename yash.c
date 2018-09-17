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
    wait((int*)NULL);
  }

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
      Yash_executeCommand(cmd);
      Command_destroy(cmd);
    }
    Vector_destroy(commands);
  }

  return 0;
}
