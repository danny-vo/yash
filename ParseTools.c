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
    // This is the program name
    if (0 == tokenCount) {
      setCommandProgram(&command, token);
      pushCommandArg(&command, token);
    // This is the arg list
    } else {
      pushCommandArg(&command, token);
    }

    token = strtok(NULL, " ");
    tokenCount++;
  }

  // exec functions expect the arg list to be null terminated
  strVecPushBack(command.arguments, NULL);
  return command;
}
