typedef enum JobState {
  BG,
  DONE,
  FG,
  NONE,
  SUS
} STATE;

typedef struct Job {
  pid_t pid;
  STATE state;
  char* process;
} Job;

Job Job_new(pid_t pid, STATE state, char* process);
void Job_destroy(Job* job);
void Job_set(Job* job, pid_t pid, STATE state, char* process);
void Job_reset(Job* job);
