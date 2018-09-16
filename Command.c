#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Command.h"
#include "Vector.h"

Command createCommand(void) {
  Command command;
  command.program = (char*)calloc(sizeof(char*), 0);
  command.arguments = (Vector*)malloc(sizeof(Vector));
  command.arguments = Vector_new(2, NULL, NULL);
  command.argLen = 0;
  return command;
}

void destroyCommand(Command command) {
  free(command.program);
  Vector_destroy(command.arguments);
}

void setCommandProgram(Command* command, char* program) {
  int progLen = strlen(program)+1;
  char* prog = (char*)malloc((progLen+1)*sizeof(char));
  strcpy(prog, program);
  command->program = prog;
  return;
}

void pushCommandArg(Command* command, char* arg) {
  char* argPtr = (char*)malloc(sizeof(char)*strlen(arg));
  strcpy(argPtr, arg);
  Vector_push(command->arguments, argPtr);
  command->argLen++;
}

void printCommand(Command command) {
  printf("################### Command ##################\n");
  printf("### Program: %s\n", command.program);
  printf("### Arguments:\n");
  printf("### ArgLen: %d\n", command.argLen);
}
