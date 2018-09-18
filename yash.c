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

  Job allTask[MAX_JOBS];
  uint32_t taskCtr;

  Job fgTask;

  uint32_t jobCnt;
} YashJobs;

/** Globals :( */
pid_t yashPid;
YashJobs yashJobs = {
  {}, 0, {}, 0,
  { 0, NONE }, 0
};

/*-------------------- Global modifier functions --------------------*/
Job* pushBgTask(void) {
  return yashJobs.bgCtr >= MAX_JOBS ? NULL : &(yashJobs.bgTasks[yashJobs.bgCtr++]);
}

Job* popBgTask(void) {
  return yashJobs.bgCtr <= 0 ? NULL : &(yashJobs.bgTasks[--yashJobs.bgCtr]);
}

Job* pushSusTask(void) {
  return yashJobs.susCtr >= MAX_JOBS ? NULL : &(yashJobs.susTasks[yashJobs.susCtr++]);
}

Job* popSusTask(void) {
  return yashJobs.susCtr <= 0 ? NULL : &(yashJobs.susTasks[--yashJobs.susCtr]);
}

void setFgTask(pid_t pid, char* process) {
  yashJobs.fgTask = Job_new(pid, FG, process);
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
  Job_set(
    pushSusTask(),
    yashJobs.fgTask.pid,
    SUS,
    yashJobs.fgTask.process
  );
  kill(yashJobs.fgTask.pid, SIGTSTP);
  Job_reset(&yashJobs.fgTask);
}

void sigchldHandler(int sigNum) {
  printf("SIGCHLD signal %d received\n", sigNum);
}

/*-------------------- Keyword command handlers -------------------*/
void sendToFg() {
  Job* susTask = popSusTask();
  Job_set(&yashJobs.fgTask, susTask->pid, FG, susTask->process);
  printf("%s\n", susTask->process);
  Job_reset(susTask);
  printf("Sending SIGCONT to %s, pid: %d\n", yashJobs.fgTask.process, yashJobs.fgTask.pid);
  kill(yashJobs.fgTask.pid, SIGCONT);
  wait((int*) NULL);
  Job_destroy(&yashJobs.fgTask);
}

void bgHandler() {

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
    Job_set(&yashJobs.fgTask, pid, FG, cmd->argStr);
    printf("Program: %s, pid: %d\n", yashJobs.fgTask.process, yashJobs.fgTask.pid);
    waitpid(-1, NULL, WUNTRACED | WCONTINUED);
    Job_reset(&yashJobs.fgTask);
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
    Job_set(&yashJobs.fgTask, pid0, FG, cmd->argStr);
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
  Job_reset(&yashJobs.fgTask);
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
        printf("argStr: %s\n", cmd->argStr);
        Yash_forkPipes(cmd);
      } else if (BG_CMD == cmd->type) {
      
      } else if (FG_CMD == cmd->type) {
        sendToFg();
      } else if (JOBS_CMD == cmd-> type) {

      } else {
        Yash_executeCommand(cmd);
        printf("argStr: %s\n", cmd->argStr);
      }
      Command_destroy(cmd);
    }

    Vector_destroy(commands);
  }

  return 0;
}
