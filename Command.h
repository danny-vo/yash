#ifndef _COMMAND_H
#define _COMMAND_H

#include "StringVector.h"

typedef struct Command {
  char* program;
  StringVector* arguments;
  int argLen;
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
Command createCommand(void);

/*
 * Function:  destroyCommand
 * -------------------------
 * Deallocates all heap pointers used by Command:
 *    program (char*), arguments (StringVector*)
 * returns: none
 */
void destroyCommand(Command command);
void setCommandProgram(Command* command, char* program);
void pushCommandArg(Command* command, char* arg);
void printCommand(Command command);

#endif /* COMMAND_H */
