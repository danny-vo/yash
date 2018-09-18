#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Command.h"
#include "Job.h"
#include "ParseTools.h"

#define MAX_JOBS 20

typedef struct YashJobs {
  Job bgTasks[MAX_JOBS];
  uint32_t bgCtr;

  Job susTasks[MAX_JOBS];
  uint32_t susCtr;

  Job fgTask;
} YashJobs;

/** Globals :( */
pid_t yashPid;
YashJobs yashJobs = {
  {}, 0, {}, 0,
  { 0, NONE }
};

/*-------------------- Global modifier functions --------------------*/
void pushBgTask(Job job) {
  if (yashJobs.bgCtr >= MAX_JOBS) { return; }
  yashJobs.bgTasks[yashJobs.bgCtr++] = job;
  return;
}

Job popBgTask(void) {
  if (yashJobs.bgCtr <= 0) { 
    Job noJob = { 0, NONE };
    return noJob;
  }
  return yashJobs.bgTasks[--yashJobs.bgCtr];
}

void pushSusTask(Job job) {
  if (yashJobs.susCtr >= MAX_JOBS) { return; }
  yashJobs.susTasks[yashJobs.susCtr++] = job;
  return;
}

Job popSusTask(void) {
  if (yashJobs.susCtr <= 0) { 
    Job noJob = { 0, NONE };
    return noJob;
  }
  return yashJobs.susTasks[--yashJobs.susCtr];
}

void printBgTasks(void) {

}

void printSusTasks(void) {

}

/*-------------------- Signal Handlers -------------------*/
void sigintHandler(int sigNum) {
  printf("Current task pid: %d\n", yashJobs.fgTask.pid);
  if (FG == yashJobs.fgTask.state) {
    kill(yashJobs.fgTask.pid, SIGINT);
    yashJobs.fgTask.state = NONE;
  }
}

void sigtstpHandler(int sigNum) {
  printf("Sending SIGTSTP to task pid: %d\n", yashJobs.fgTask.pid);
  Job susJob = {
    yashJobs.fgTask.pid,
    SUS
  };
  pushSusTask(susJob);
  kill(yashJobs.fgTask.pid, SIGTSTP);
  yashJobs.fgTask.state = NONE;
}

void sigchldHandler(int sigNum) {
  printf("SIGCHLD signal %d received\n", sigNum);
}

/*-------------------- Keyword command handlers -------------------*/
void fgHandler() {
  Job susTask = popSusTask();
  susTask.state = FG;
  yashJobs.fgTask = susTask;
  printf("Sending SIGCONT to pid: %d\n", yashJobs.fgTask.pid);
  yashJobs.fgTask.state = FG;
  kill(yashJobs.fgTask.pid, SIGCONT);
  wait((int*) NULL);
  yashJobs.fgTask.state = NONE;
}

/*-------------------- Shell operations --------------------*/
void Yash_redirect(Command* cmd) {
  /* Redirect stdin */
  if (cmd->fdTable.stdIn) {
    int ifd = open(
      cmd->fdTable.stdIn,
      O_RDONLY,
      S_IRGRP| S_IROTH | S_IRUSR | S_IWUSR
    );
    dup2(ifd, STDIN_FILENO);
    close(ifd);
  }

  /* Redirect stdout */
  if (cmd->fdTable.stdOut) {
    int ofd = open(
      cmd->fdTable.stdOut,
      O_CREAT | O_WRONLY,
      S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR
    );
    dup2(ofd, STDOUT_FILENO);
    close(ofd);
  }

  /* Redirect stderr */
  if (cmd->fdTable.stdErr) {
    int efd = open(
      cmd->fdTable.stdErr,
      O_CREAT | O_WRONLY,
      S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR
    );
    dup2(efd, STDERR_FILENO);
    close(efd);
  }

  return;
}

void Yash_executeCommand(Command* cmd) {
  pid_t pid = fork();

  /* Child executes the program */
  if (0 == pid) {
    Yash_redirect(cmd);
    execvp(cmd->program, Command_getArgs(cmd));
  /* Error */
  } else if (-1 == pid) {
    perror("fork() error\n");
  /* Blocking until child finishes */
  } else {
    yashJobs.fgTask.pid = pid;
    yashJobs.fgTask.state = FG;
    printf("Program: %s, pid: %d\n", cmd->program, yashJobs.fgTask.pid);
    int chldStatus;
    waitpid(-1, &chldStatus, WUNTRACED | WCONTINUED);
  }
  
}

int Yash_forkPipes(Command* cmd) {
  int pipeFd[2];
  pipe(pipeFd);

  /* First child */
  pid_t pid0 = fork();
  if (0 == pid0) {
    setpgid(0,0);
    close(pipeFd[0]);
    dup2(pipeFd[1], STDOUT_FILENO);
    Yash_redirect(cmd->pipe[0]);
    execvp(cmd->pipe[0]->program, Command_getArgs(cmd->pipe[0]));
  } else if (pid0 < 0) {
    perror("fork() error\n");
  /* Parent process assigns stuff */
  } else {
    yashJobs.fgTask.pid = pid0;
    yashJobs.fgTask.state = FG;
  }
 
  /* Second child */
  pid_t pid1 = fork();
  if (0 == pid1) {
    setpgid(0, pid0);
    close(pipeFd[1]);
    dup2(pipeFd[0], STDIN_FILENO);
    Yash_redirect(cmd->pipe[1]);
    execvp(cmd->pipe[1]->program, Command_getArgs(cmd->pipe[1]));
  } else if (pid1 < 0) {
    perror("fork() error\n");
  }

  /* Cleanup pipes */
  close(pipeFd[0]);
  close(pipeFd[1]);
  waitpid(pid0, NULL, WUNTRACED|WCONTINUED);
  waitpid(pid1, NULL, WUNTRACED|WCONTINUED);
  return 0;
}

int main(int argc, char* argv[]) {
  /* Assign signal handlers */
  signal(SIGINT, sigintHandler);
  signal(SIGTSTP, sigtstpHandler);
  //signal(SIGCHLD, sigchldHandler);

  char* inString;
  while (inString = readline("# ")) {
    Vector* commands = Parse_commands(inString);

    while (Vector_size(commands) > 0) {
      Command* cmd = Vector_pop(commands);

      if (PIPE == cmd->type) {
        Yash_forkPipes(cmd);
      } else if (BG_CMD == cmd->type) {
      
      } else if (FG_CMD == cmd->type) {
        fgHandler();
      } else if (JOBS_CMD == cmd-> type) {

      } else {
        Yash_executeCommand(cmd);
      }
      Command_destroy(cmd);
    }

    Vector_destroy(commands);
  }

  return 0;
}
