#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "ParseTools.h"

void forkExecvp(char* cmdProg, char** cmdArgs) {
  pid_t pid = fork();

  if (0 == pid) {
    execvp(cmdProg, cmdArgs);
  } else if (-1 == pid) {
    perror("fork() error\n");
  }

}

int main(int argc, char* argv[]) {
  char* inString;
  
  while (inString = readline("yash> ")) {
    Command command = parseCommand(inString);
    forkExecvp(command.program, (command.arguments)->data);
    destroyCommand(command);
  }

  return 0;
}
