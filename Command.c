#include <stdlib.h>

#include "Command.h"

Command createCommand(void) {
  Command command;
  command.program = (char*)calloc(sizeof(char*), 0);
  command.arguments = (StringVector*)malloc(sizeof(StringVector));
  *(command.arguments) = createStrVec(0);
  command.argLen = 0;
  return command;
}

void destroyCommand(Command command) {
  free(command.program);
  destroyStrVec(*(command.arguments));
}

void setCommandProgram(Command* command, char* program) {
  int progLen = 0;
  while (program[progLen] != '\0') {
    ++progLen;
  }
  char* prog = (char*)calloc((sizeof(char)*progLen)+1, *program);
  command->program = prog;
  return;
}

void pushCommandArg(Command* command, char* arg) {
  strVecPushBack(command->arguments, arg);
  command->argLen++;
}
