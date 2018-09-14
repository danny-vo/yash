typedef struct Command {
  char* program;
  char** arguments;
  int argLen;
} Command;

char** parseString(char* str);

Command parseCommand(char** str);
