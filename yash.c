#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>


#include "ParseTools.h"

int main(int argc, char* argv[]) {
  char* inString;
  
  while (inString = readline("yash> ")) {
    Command command = parseCommand(inString);
    printCommand(command);
    destroyCommand(command);
  }

  return 0;
}
