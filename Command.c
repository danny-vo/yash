#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Command.h"
#include "StringVector.h"

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
  free(command.arguments);
}

void setCommandProgram(Command* command, char* program) {
  int progLen = strlen(program)+1;
  char* prog = (char*)malloc((progLen+1)*sizeof(char));
  strcpy(prog, program);
  command->program = prog;
  return;
}

void pushCommandArg(Command* command, char* arg) {
  strVecPushBack(command->arguments, arg);
  command->argLen++;
}

void printCommand(Command command) {
  printf("################### Command ##################\n");
  printf("### Program: %s\n", command.program);
  printf("### Arguments:\n");
  strVecPrint(*command.arguments);
  printf("### ArgLen: %d\n", command.argLen);
}
