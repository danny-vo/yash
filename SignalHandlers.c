#include <stdio.h>

void sigintHandler(int sigNum) {
  printf("SIGINT signal %d received\n", sigNum);
}

void sigtstpHandler(int sigNum) {
  printf("SIGTSTP signal %d received\n", sigNum);
}

void sigchldHandler(int sigNum) {
  printf("SIGCHLD signal %d received\n", sigNum);
}
