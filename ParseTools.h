#ifndef _PARSETOOLS_H
#define _PARSETOOLS_H

#include "Command.h"

char** parseString(char* str);
Command parseCommand(char* str);
void destroyCommand(Command);

#endif /* PARSETOOLS_H */
