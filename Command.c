#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Command.h"
#include "Vector.h"


Command* Command_new(void) {
  Command* cmd = (Command*)calloc(1, sizeof(Command));
  cmd->program = NULL;
  cmd->arguments = (Vector*)malloc(sizeof(Vector));
  cmd->arguments = Vector_new(2, NULL, NULL);
  cmd->argLen = 0;
  return cmd;
}

void destroyFdTable(FileDescriptorTable* fdTable) {
  if (fdTable->stdIn) free(fdTable->stdIn);
  if (fdTable->stdOut) free(fdTable->stdOut);
  if (fdTable->stdErr) free(fdTable->stdErr);
  return;
}
void Command_destroy(Command* cmd) {
  if (NULL != cmd->program) free(cmd->program);
  if (PIPE == cmd->type) {
    Command_destroy(cmd->pipe[0]);
    Command_destroy(cmd->pipe[1]);
  }
  destroyFdTable(&(cmd->fdTable));
  Vector_destroy(cmd->arguments);
  free(cmd);
}

void Command_setProgram(Command* cmd, char* program) {
  int progLen = strlen(program)+1;
  char* prog = (char*)malloc((progLen+1)*sizeof(char));
  strcpy(prog, program);
  cmd->program = prog;
  return;
}

void Command_pushArg(Command* cmd, char* arg) {
  char* argPtr = (char*)malloc(sizeof(char)*strlen(arg));
  strcpy(argPtr, arg);
  Vector_push(cmd->arguments, argPtr);
  cmd->argLen++;
}

char** Command_getArgs(Command* cmd) {
  return (char**)cmd->arguments->data;
}

void Command_print(Command cmd) {
  printf("################### Command ##################\n");
  printf("### Program: %s\n", cmd.program);
  printf("### Arguments:\n");
  printf("### ArgLen: %d\n", cmd.argLen);
}
