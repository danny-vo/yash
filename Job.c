#include <stdlib.h>
#include <string.h>

#include "Job.h"

Job Job_new(pid_t pid, STATE state, char* process) {
  Job job;
  job.pid = pid;
  job.state = state;
  job.process = malloc(sizeof(char)*strlen(process));
  strcpy(job.process, process);
}

void Job_destroy(Job* job) {
  job->pid = 0;
  job->state = NONE;
  if (job->process) free(job->process);
  job->process = NULL;
}

void Job_set(Job* job, pid_t pid, STATE state, char* process) {
  job->pid = pid;
  job->state = state;
  if (!job->process) free(job->process);
  char* str = malloc((sizeof(char) * strlen(process)) + 1);
  strcpy(str, process);
  job->process = str;
}

void Job_reset(Job* job) {
  job->pid = 0;
  job->state = NONE;
  job->process = NULL;
}
