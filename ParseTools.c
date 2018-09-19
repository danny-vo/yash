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

void Parse_setRedirection(Vector* tokens, uint32_t* tokPos, Command* cmd) {
  char* token = Vector_getElem(tokens, *tokPos);

  /* Change stdin */
  if (!strcmp(token, "<")) {
    Command_buildArgStr(cmd, token);
    char* file = Vector_getElem(tokens, ++(*tokPos));
    Command_buildArgStr(cmd, file);
    cmd->fdTable.stdIn = malloc(sizeof(char)*strlen(file));
    strcpy(cmd->fdTable.stdIn, file);
  /* Change stdout */
  } else if (!strcmp(token, ">")) {
    Command_buildArgStr(cmd, token);
    char* file = Vector_getElem(tokens, ++(*tokPos));
    Command_buildArgStr(cmd, file);
    cmd->fdTable.stdOut = malloc(sizeof(char)*strlen(file));
    strcpy(cmd->fdTable.stdOut, file);
  /* Change stderr */
  } else if (!strcmp(token, "2>")){
    Command_buildArgStr(cmd, token);
    char* file = Vector_getElem(tokens, ++(*tokPos));
    Command_buildArgStr(cmd, file);
    cmd->fdTable.stdErr = malloc(sizeof(char)*strlen(file));
    strcpy(cmd->fdTable.stdErr, file);
  }

  return;
}

Command* Parse_directive(Vector* tokens, uint32_t* tokPos) {
  Command* cmd = Command_new();
  char* token = Vector_getElem(tokens, *tokPos);
  int tokenCount = 0;

  /* Parse out the cmd and its arguments */
  while(*tokPos < Vector_size(tokens) && strcmp(token, "|")) {

    /* Name of executable */
    if (0 == tokenCount) {
      Command_setProgram(cmd, token);
      Command_pushArg(cmd, token);
      Command_buildArgStr(cmd, token);
    /* Argument list */
    } else {
      /* Handle redirection */
      if(!(strcmp(token, "<")) || !strcmp(token, ">") || !strcmp(token, "2>")) {
        Parse_setRedirection(tokens, tokPos, cmd);
      
      } else if (!strcmp(token, "&")) {
        Command_buildArgStr(cmd, token);
        cmd->isBgTask = 1;
      } else {
        Command_pushArg(cmd, token);
        Command_buildArgStr(cmd, token);
      }
    }
    
    token = Vector_getElem(tokens, ++(*tokPos));
    tokenCount++;
  }

  /* exec functions expect the arg list to be null terminated */
  Vector_push(cmd->arguments, NULL);
  return cmd;
}

Command* Parse_pipe(Vector* cmds, Vector* tokens, uint32_t* tokPos) {
  Command* pipeCmd = Command_new();
  pipeCmd->pipe[0] = Vector_pop(cmds);

  /* Pipes have two components, command preceding it is first */
  (*tokPos)++;
  pipeCmd->pipe[1] = Parse_directive(tokens, tokPos);
  Command_buildArgStr(pipeCmd, pipeCmd->pipe[0]->argStr);
  Command_buildArgStr(pipeCmd, "|");
  Command_buildArgStr(pipeCmd, pipeCmd->pipe[1]->argStr);
  pipeCmd->isBgTask = pipeCmd->pipe[1]->isBgTask;

  return pipeCmd;
}

Vector* Parse_commands(char* str) {
  Vector* commands = Vector_new(0, (void (*)(void*))&Command_destroy, NULL);
  Vector* tokens = Parse_tokens(str);
  uint32_t tokPos = 0;
  
  while (tokPos < Vector_size(tokens)) {
    /* Piping */
    if (!strcmp(Vector_getElem(tokens, tokPos), "|")) {
      Command* pipeCmd = Parse_pipe(commands, tokens, &tokPos);
      pipeCmd->type = PIPE;
      Vector_push(commands, pipeCmd);
    } else if (!strcmp(Vector_getElem(tokens, tokPos), "bg")) {
      Command* bgCmd = Command_new();
      bgCmd->type = BG_CMD;
      Vector_push(commands, bgCmd);
      tokPos++;
    } else if (!strcmp(Vector_getElem(tokens, tokPos), "fg")) {
      Command* fgCmd = Command_new();
      fgCmd->type = FG_CMD;
      Vector_push(commands, fgCmd);
      tokPos++;
    } else if (!strcmp(Vector_getElem(tokens, tokPos), "jobs")) {
      Command* jobsCmd = Command_new();
      jobsCmd->type = JOBS_CMD;
      Vector_push(commands, jobsCmd);
      tokPos++;
    /* Executable */
    } else {
      Command* exeCmd = Parse_directive(tokens, &tokPos);
      exeCmd->type = EXECUTABLE;
      Vector_push(commands, exeCmd);
    }

  }

  Vector_destroy(tokens);
  return commands;
}
