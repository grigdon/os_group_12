#pragma once
#include <sys/types.h>

typedef struct {
    int job_id;
    pid_t pid;
    char *cmdline;
    int active;
} job_t;

extern job_t jobs[10];
extern int job_count;

void add_job(pid_t pid, const char *cmdline);
void check_jobs(void);
