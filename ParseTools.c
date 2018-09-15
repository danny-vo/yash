#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ParseTools.h"

char** parseString(char* str) {
  char* token;
  token = strtok(str, " ");

  while (token != NULL) {
    token = strtok(NULL, " ");
  }
}

Command parseCommand(char* str) {
  // Allocates the Command struct
  Command command = createCommand();

  // Parse out the command and its arguments
  char* token;
  int tokenCount = 0;
  token = strtok(str, " ");
  while(token != NULL) {
    if (0 == tokenCount) {
      setCommandProgram(&command, token);
    } else {
      pushCommandArg(&command, token);
    }
    token = strtok(NULL, " ");
    tokenCount++;
  }

  return command;
}
