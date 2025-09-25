#include "jobs.h"
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

job_t jobs[10] = {0};
int job_count = 0;

static int next_job_id = 1;

static int find_slot(void) {
    for (int i = 0; i < 10; i++) {
        if (!jobs[i].active) {
            return i;
        } 
    }
    return -1;
}

static int job_by_pid(pid_t pid) {
    for (int i = 0; i < 10; ++i) {
        if (jobs[i].active && jobs[i].pid == pid) {
            return i;
        }  
    }
    return -1;
}

void add_job(pid_t pid, const char * cmdline) {
    if (job_count >= 10) {
        fprintf(stderr, "Error: Too many background jobs (max is 10)\n");
        return;
    }

    int index = find_slot();
    if (index < 0) {
        fprintf(stderr, "Error: No free job slot\n");
        return;
    }

    jobs[index].active = 1;
    jobs[index].job_id = next_job_id++;
    jobs[index].pid = pid;
    if (cmdline != NULL) {
        jobs[index].cmdline = str_dup(cmdline);
    } else {
        jobs[index].cmdline = NULL;
    }
    job_count++;

    printf("[%d] %d\n", jobs[index].job_id, (int)jobs[index].pid);
    fflush(stdout);
}

void check_jobs(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        int index = job_by_pid(pid);

        if (index >= 0) {
            if (jobs[index].cmdline) {
                printf("[%d] + done %s\n", jobs[index].job_id, jobs[index].cmdline);
            } else {
                printf("[%d] + done\n", jobs[index].job_id);
            }
            fflush(stdout);

            free(jobs[index].cmdline);
            jobs[index].cmdline = NULL;
            jobs[index].active = 0;
            jobs[index].pid = 0;
            job_count--;
        }
    }
}
