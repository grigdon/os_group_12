#include "io_redirection.h"
#include "shell.h"
#include "helper.h"
#include "lexer.h"
#include "jobs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> // pid_t type
#include <sys/wait.h> // wait()
#include <sys/stat.h> // S_ISDIR
#include <errno.h>

char *search_path(char* command) {
    // case 1: explicit executable, i.e, in_token="/usr/bin/ls"
    if(strchr(command, '/') != NULL) { // check for any '/' in command string
        if(access(command, X_OK) == 0) {
            return str_dup(command); 
        } else {
            return NULL; // if file does not exist or is not executable, return NULL
        }
    // case 2: search required, i.e., in_token="ls"
    } else {
        char * path_env = getenv("PATH");
        // Added missing PATH handling
        if (!path_env || !*path_env) return NULL;
        char * path_copy = str_dup(path_env);

        char* directory = strtok(path_copy, ":"); // get first directory from PATH 
        char full_path[1024];

        while(directory != NULL) {
            snprintf(full_path, sizeof(full_path), "%s/%s", directory, command);

            if(access(full_path, X_OK) == 0) {
                free(path_copy);
                return str_dup(full_path);
            }
            // go to next token        
            directory = strtok(NULL, ":");
        }
        free(path_copy);
        return NULL; // if not matches after loop, return NULL
    }
} 

extern char **environ; // passes environment over

// Creates new token list that's argv only
static tokenlist * take_redirections(const tokenlist * tokens, io_redirection_t * out_r) {
    tokenlist * argv_only = new_tokenlist();
    out_r->in_path = NULL;
    out_r->out_path = NULL;

    for (size_t i = 0; i < tokens->size; i++) {
        char * t = tokens->items[i];

        //
        if (strcmp(t, "<") == 0) {
            if (i + 1 >= tokens->size) {
                fprintf(stderr, "Error: No input file after '<'\n");
                free_tokens(argv_only);
                return NULL;
            }
            out_r->in_path = tokens->items[i + 1];
            i++;
            continue;
        }

        // >
        if (strcmp(t, ">") == 0) {
            if (i + 1 >= tokens->size) {
                fprintf(stderr, "Error: No output file after '>'\n");
                free_tokens(argv_only);
                return NULL;
            }
            out_r->out_path = tokens->items[i + 1];
            i++;
            continue;
        }

        // Normal argument
        add_token(argv_only, t);
    }

    return argv_only;
}

static int count_pipes(const tokenlist * tokens) {
    int count = 0;
    for (size_t i = 0; i < tokens->size; i++) {
        if (strcmp(tokens->items[i], "|") == 0) {
            count++;
        }
    }
    return count;
}


// Builds tokenlists for each command split by pipes
static int split_by_pipes(const tokenlist *tokens, tokenlist ***parts_out) {
    int capacity = 4;
    int part_index = 0;
    tokenlist **parts = malloc(sizeof(tokenlist*) * capacity);
    if (!parts) {
        perror("malloc");
        return -1;
    }
    parts[part_index] = new_tokenlist();

    for (size_t i = 0; i < tokens->size; i++) {
        if (strcmp(tokens->items[i], "|") == 0) {
            if (parts[part_index]->size == 0) {
                fprintf(stderr, "Error: No command between pipes\n");
                for (int j = 0; j <= part_index; j++) {
                    free_tokens(parts[j]);
                }
                free(parts);
                return -1;
            }

            part_index++;

            if (part_index >= capacity) {
                capacity *= 2;
                tokenlist **new_parts = realloc(parts, sizeof(tokenlist*) * capacity);
                if (!new_parts) {
                    perror("realloc");
                    for (int j = 0; j <= part_index; j++) {
                        free_tokens(parts[j]);
                    }
                    free(parts);
                    return -1;
                }
                parts = new_parts;
            }

            parts[part_index] = new_tokenlist();
            continue;
        }

        add_token(parts[part_index], tokens->items[i]);
    }

    if (parts[part_index]->size == 0) {
        fprintf(stderr, "Error: No command after the last pipe\n");
        for (int j = 0; j <= part_index; j++) {
            free_tokens(parts[j]);
        }
        free(parts);
        return -1;
    }

    *parts_out = parts;
    return part_index + 1;
}

// Run pipeline in background. Do not wait if bg is 1. Wait if bg is 0.
static void execute_pipeline_bg(tokenlist **parts, int parts_count, int background, pid_t *out_last_pid) {
    char **paths = malloc(sizeof(char*) * parts_count);
    pid_t *pids = malloc(sizeof(pid_t) * parts_count);
    if (!paths || !pids) {
        perror("malloc");
        free(paths);
        free(pids);
        return;
    }

    for (int i = 0; i < parts_count; i++) {
        paths[i] = search_path(parts[i]->items[0]);
        if (!paths[i]) {
            fprintf(stderr, "Command not found: %s\n", parts[i]->items[0]);
            for (int k = 0; k < i; k++) {
                free(paths[k]);
            }
            free(paths);
            free(pids);
            return;
        }
    }

    int prev_pipefd = -1;
    int pipefd[2];

    for (int i = 0; i < parts_count; i++) {
        if (i < parts_count - 1 && pipe(pipefd) < 0) {
            perror("pipe");
            if (prev_pipefd != -1) {
                close(prev_pipefd);
            }

            for (int j = 0; j < i; j++) {
                if (pids[j] > 0) {
                    waitpid(pids[j], NULL, 0);
                }
            }

            for (int j = 0; j < parts_count; j++) {
                free(paths[j]);
            }
            free(paths);
            free(pids);
            return;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            if (prev_pipefd != -1) {
                close(prev_pipefd);
            }

            if (i < parts_count - 1) {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            for (int j = 0; j < i; j++) {
                if (pids[j] > 0) {
                    waitpid(pids[j], NULL, 0);
                }
            }

            for (int j = 0; j < parts_count; j++) {
                free(paths[j]);
            }
            free(paths);
            free(pids);
            return;
        }

        if (pid == 0) {
            if (prev_pipefd != -1) {
                if (dup2(prev_pipefd, STDIN_FILENO) < 0) {
                    perror("dup2 stdin"); _exit(1);
                }
            }
            if (i < parts_count - 1) {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
                    perror("dup2 stdout"); _exit(1);
                }
            }

            if (prev_pipefd != -1) close(prev_pipefd);
            if (i < parts_count - 1) { close(pipefd[0]); close(pipefd[1]); }

            execv(paths[i], parts[i]->items);
            perror("execv");
            _exit(127);
        } else {
            pids[i] = pid;

            if (prev_pipefd != -1) {
                close(prev_pipefd); prev_pipefd = -1;
            }
            if (i < parts_count - 1) {
                close(pipefd[1]);
                prev_pipefd = pipefd[0];
            }
        }
    }

    if (prev_pipefd != -1) {
        close(prev_pipefd);
    }

    if (out_last_pid) * out_last_pid = pids[parts_count - 1];

    if (!background) {
        for (int i = 0; i < parts_count; i++) {
            int status;
            waitpid(pids[i], &status, 0);
            if (!WIFEXITED(status)) {
                perror("Pipeline child stopped");
            }
        }
    }

    for (int i = 0; i < parts_count; i++) {
        free(paths[i]);
    }
    free(paths);
    free(pids);
}

static int remove_last_ampersand(tokenlist *tokens) {
    if (tokens->size == 0) return 0;

    if (strcmp(tokens->items[tokens->size - 1], "&") == 0) {
        free(tokens->items[tokens->size - 1]);
        tokens->items[tokens->size - 1] = NULL;
        tokens->size -= 1;
        return 1;
    }
    return 0;
}

static char * join_tokens(const tokenlist * tokens) {
    size_t total = 0;
    for (size_t i = 0; i < tokens->size; ++i) {
        total += strlen(tokens->items[i]) + 1;
    }

    if (total == 0) return NULL;
    char * buffer = malloc(total + 1);
    if (!buffer) return NULL;
    buffer[0] = '\0';
    for (size_t i = 0; i < tokens->size; i++) {
        strcat(buffer, tokens->items[i]);
        if (i + 1 < tokens->size) strcat(buffer, " ");
    }
    return buffer;
}

static char *g_command_history[3] = {NULL, NULL, NULL}; // stores (up to) last three commands
static int g_history_count = 0;
static int g_history_next = 0;

void add_to_history(const char *cmdline) {
    if (!cmdline || cmdline[0] == '\0') {
        return;
    }
    if (g_command_history[g_history_next]) {
        free(g_command_history[g_history_next]);
    }
    g_command_history[g_history_next] = str_dup(cmdline);
    g_history_next = (g_history_next + 1) % 3;
    if (g_history_count < 3) {
        g_history_count++;
    }
}

void print_history(void) {
    if (g_history_count == 0) {
        printf("No valid commands in history.\n");
        return;
    }

    printf("Last %d commands:\n", g_history_count);
    for (int i = 0; i < g_history_count; i++) {
        int index = (g_history_next - g_history_count + i + 3) % 3;
        printf("%s\n", g_command_history[index]);
    }
}

void execute_command(tokenlist * tokens) {
    char *full_cmd_str = join_tokens(tokens);

    // check if single token is a directory
    if (tokens->size == 1) {
        char *potential_path = tokens->items[0];
        struct stat stat_buf;

        // if path exists
        if (stat(potential_path, &stat_buf) == 0) {
            // write bash output if true
            if (S_ISDIR(stat_buf.st_mode)) {
                fprintf(stderr, "bash: %s: Is a directory\n", potential_path);
                free(full_cmd_str);
                return;
            }
            // if exists but not dir nor exec
            else if (access(potential_path, X_OK) != 0) {
                fprintf(stderr, "bash: %s: Permission denied\n", potential_path);
                free(full_cmd_str);
                return;
            }
        }
    }

    // case when input == exit
    if (strcmp(tokens->items[0], "exit") == 0) {
        while (job_count > 0) {
            printf("Waiting for %d background job(s) to finish...\n", job_count);
            check_jobs();
            sleep(1);
        }
        print_history();
        for(int i = 0; i < 3; i++) {
            if(g_command_history[i]) free(g_command_history[i]);
        }
        free(full_cmd_str);
        exit(0);
    }

    // case when input == cd
    if (strcmp(tokens->items[0], "cd") == 0) {
        char* path = NULL;
        if (tokens->size == 1) { // "cd" with no arguments
            path = getenv("HOME");
            if (path == NULL) {
                fprintf(stderr, "cd: HOME not set\n");
            }
        } else if (tokens->size == 2) { // "cd" with one argument
            path = tokens->items[1];
        } else { // "cd" with too many arguments, e.g. "cd ./os_group_12 ./obj"
            fprintf(stderr, "cd: too many arguments\n");
        }

        if (path && chdir(path) != 0) {
            perror("cd");
        }
        free(full_cmd_str);
        return;
    }

    // case when input == jobs
    if (strcmp(tokens->items[0], "jobs") == 0) {
        if (job_count == 0) {
            printf("No active background processes.\n");
        } else {
            for (int i = 0; i < 10; i++) {
                if (jobs[i].active) {
                    printf("[%d]+ %d %s\n", jobs[i].job_id, jobs[i].pid, jobs[i].cmdline);
                }
            }
        }
        free(full_cmd_str);
        return;
    }

    add_to_history(full_cmd_str); // add command to history AFTER running 'exit'

    int is_bg = remove_last_ampersand(tokens);

    // Pipeline branch
    int pipes_count = count_pipes(tokens);
    if (pipes_count > 0) {
        tokenlist **parts = NULL;
        int parts_count = split_by_pipes(tokens, &parts);
        if (parts_count < 0) {
            free(full_cmd_str);
            return;
        }

        pid_t last_pid = -1;
        execute_pipeline_bg(parts, parts_count, is_bg, &last_pid);

        if (is_bg && last_pid > 0) {
            add_job(last_pid, full_cmd_str);
        }

        for (int i = 0; i < parts_count; ++i) {
            free_tokens(parts[i]);
        }
        free(parts);
        free(full_cmd_str);
        return;
    }

    // No pipes
    io_redirection_t redir;
    tokenlist * argv_only = take_redirections(tokens, &redir);
    if (!argv_only) {
        free(full_cmd_str);
        return;
    }
    if (argv_only->size == 0) {
        free_tokens(argv_only);
        free(full_cmd_str);
        return;
    }

    char * full_path = search_path(argv_only->items[0]);
    if (full_path == NULL) {
        fprintf(stderr, "command not found: %s\n", argv_only->items[0]);
        free_tokens(argv_only);
        free(full_cmd_str);
        return;
    }

    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        free(full_path);
        free_tokens(argv_only);
        free(full_cmd_str);
        return;
    } else if (child_pid == 0) {
        if (apply_io_redirection(&redir) < 0) _exit(1);
        execv(full_path, argv_only->items);
        perror("error with executing command");
        _exit(127);
    } else {
        if (is_bg) {
            add_job(child_pid, full_cmd_str);
        } else {
            int status;
            waitpid(child_pid, &status, 0);
            if (!WIFEXITED(status)) {
                perror("Child terminated with abnormal status WIFEXITED(STATUS)");
            }
        }
        free(full_cmd_str);
    }

    free(full_path);
    free_tokens(argv_only);
}