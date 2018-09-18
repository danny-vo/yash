typedef enum JobState {
  BG,
  FG,
  NONE,
  SUS
} STATE;

typedef struct Job {
  pid_t pid;
  STATE state;
} Job;
