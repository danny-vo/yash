#ifndef _COMMAND_H
#define _COMMAND_H

#include "Vector.h"

typedef enum CommandType {
  EXECUTABLE,
  REDIRECTION,
  PIPE
} TYPE;

typedef struct Command {
  char* program;
  Vector* arguments;
  int argLen;
  TYPE type;
} Command;

/*
 * Function:  createCommand
 * ------------------------
 * Initializes a Command struct
 * Allocates a pointer for a char string
 * Allocates a pointer for a StringVector
 * Initializes the argument length metadata to 0
 *
 * returns: An initalized Command struct
 */
Command* Command_new(void);

/*
 * Function:  destroyCommand
 * -------------------------
 * Deallocates all heap pointers used by Command:
 *    program (char*), arguments (StringVector*)
 * returns: none
 */
void Command_destroy(Command* command);
void Command_setProgram(Command* command, char* program);
void Command_setType(Command* command, TYPE type);
void Command_pushArg(Command* command, char* arg);
void Command_print(Command command);

#endif /* COMMAND_H */
