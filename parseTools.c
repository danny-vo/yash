#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseTools.h"

char** parseString(char* str) {
  char* token;
  token = strtok(str, " ");

  while (token != NULL) {
    printf(" %s\n", token);
    token = strtok(NULL, " ");
  }
}

Command parseCommand(char** str) {
  Command* command = (Command*)malloc(sizeof(Command));
}
