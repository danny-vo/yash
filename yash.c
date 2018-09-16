#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#include "ParseTools.h"
#include "SignalHandlers.h"

void forkExecvp(char* cmdProg, char** cmdArgs) {
  pid_t pid = fork();

  // Child executes the program
  if (0 == pid) {
    execvp(cmdProg, cmdArgs);
  // Error
  } else if (-1 == pid) {
    perror("fork() error\n");
  // Blocking until child finishes
  } else {
    wait((int*)NULL);
  }

}

int redirectOutput(char* outTarget) {
  int fd = open(outTarget, "w");
  return fd;
}

int redirectInput(char* inTarget) {
  int fd = open(inTarget, "r");
  return fd;
}

int main(int argc, char* argv[]) {
  /* Assign signal handlers */
  //signal(SIGINT, sigintHandler);
  signal(SIGTSTP, sigtstpHandler);
  signal(SIGCHLD, sigchldHandler);
  
  char* inString;
  while (inString = readline("# ")) {
    Command command = parseCommand(inString);
    forkExecvp(command.program, (char**)(command.arguments)->data);
    destroyCommand(command);
  }

  return 0;
}
