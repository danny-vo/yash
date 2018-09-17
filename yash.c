#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Command.h"
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

#if 0
int redirectOutput(char* outTarget) {
  int fd = open(outTarget, "w");
  return fd;
}

int redirectInput(char* inTarget) {
  int fd = open(inTarget, "r");
  return fd;
}
#endif

void Yash_commandHandler(Command* cmd) {
  if (EXECUTABLE == cmd->type) {
    forkExecvp(cmd->program, (char**)(cmd->arguments)->data);
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
      Yash_commandHandler(cmd);
      Command_destroy(cmd);
    }
    Vector_destroy(commands);
  }

  return 0;
}
