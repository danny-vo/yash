#ifndef _SIGNALHANDLERS_H
#define _SIGNALHANDLERS_H

void sigintHandler(int sigNum);
void sigtstpHandler(int sigNum);
void sigchldHandler(int sigNum);

#endif /* SIGNALHANDLERS_H */
