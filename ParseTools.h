#ifndef _PARSETOOLS_H
#define _PARSETOOLS_H

#include "Command.h"

char** parseString(char* str);
Vector* Parse_commands(char* str);
void destroyCommand(Command);

#endif /* PARSETOOLS_H */
