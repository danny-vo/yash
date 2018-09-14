#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>


#include "ParseTools.h"

int main() {
  char* inString;
  char** parseCmd;
  
  while (inString = readline("yash> ")) {
    Command command = parseCommand(inString);
    printf("Command: %s\n", command.program);
    destroyCommand(command);
  }

  return 0;
}
