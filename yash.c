#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>


#include "parseTools.h"

int main() {
  char* inString;
  char** parseCmd;
  
  while (inString = readline("yash> ")) {
    char** parseCmd = parseString(inString);
  }

  return 0;
}
