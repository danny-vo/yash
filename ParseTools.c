#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ParseTools.h"
#include "Vector.h"

char** parseString(char* str) {
  char* token;
  token = strtok(str, " ");

  while (token != NULL) {
    token = strtok(NULL, " ");
  }
}

Vector* Parse_tokens(char* str) {
  Vector* tokens = Vector_new(0, NULL, NULL);
  char* tok = strtok(str, " ");

  while (NULL != tok) {
    char* tokPtr = (char*)malloc(sizeof(char)*strlen(tok));
    strcpy(tokPtr, tok);
    Vector_push(tokens, tokPtr);
    tok = strtok(NULL, " ");
  }

  return tokens;
}

Command* Parse_directive(Vector* tokens, uint32_t* tokPos) {
  // Allocates the Command struct
  Command* cmd = Command_new();
  
  char* token = Vector_getElem(tokens, *tokPos);
  int tokenCount = 0;
  // Parse out the cmd and its arguments
  while(*tokPos < Vector_size(tokens) && (strcmp(token, "<"))
        && strcmp(token, ">") && strcmp(token, "2>") && strcmp(token, "|")) {

    // This is the program name
    if (0 == tokenCount) {
      Command_setProgram(cmd, token);
      Command_pushArg(cmd, token);
    // This is the arg list
    } else {
      Command_pushArg(cmd, token);
    }
    
    token = Vector_getElem(tokens, ++(*tokPos));
    tokenCount++;
  }

  // exec functions expect the arg list to be null terminated
  Vector_push(cmd->arguments, NULL);
  return cmd;
}

Vector* Parse_commands(char* str) {
  Vector* commands = Vector_new(0, (void (*)(void*))&Command_destroy, NULL);
  Vector* tokens = Parse_tokens(str);
  uint32_t tokPos = 0;
  
  while (tokPos < Vector_size(tokens)) {
    // Handle redirecting stdin/stdout/stderr
    if (!strcmp(Vector_getElem(tokens, tokPos), "<")
        || !strcmp(Vector_getElem(tokens, tokPos),">")
        || !strcmp(Vector_getElem(tokens, tokPos),"2>")) {
      

    // Handle piping
    } else if (!strcmp(Vector_getElem(tokens, tokPos), "|")) {

    // Exectuables
    } else {
      Command* exeCmd = Parse_directive(tokens, &tokPos);
      Vector_push(commands, exeCmd);
    }

  }

  Vector_destroy(tokens);
  return commands;
}
