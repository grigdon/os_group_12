#include "io_redirection.h"
#include "shell.h"
#include "helper.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> // pid_t type
#include <sys/wait.h> // wait()

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
        char* path_copy = str_dup(getenv("PATH")); // store $PATH
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

        // <
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


// Updated to use the new argv_only list
void execute_command(tokenlist *tokens) {
    io_redirection_t redir;
    tokenlist * argv_only = take_redirections(tokens, &redir);

    if (!argv_only) {
        return;
    }

    if (argv_only->size == 0) {
        free_tokens(argv_only);
        return;
    }

    char *full_path = search_path(argv_only->items[0]);
    if (full_path == NULL) {
        perror("command does not exist");
        free_tokens(argv_only);
        return;
    }

    int status;
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        free(full_path);
        free_tokens(argv_only);
        return;
    } else if (child_pid == 0) {
        if (apply_io_redirection(&redir) < 0) {
            _exit(1);
        }

        execv(full_path, argv_only->items);
        perror("error with executing command");
        _exit(127);
    } else {
        waitpid(child_pid, &status, 0);
        if (!(WIFEXITED(status))) {
            perror("Child terminated with abnormal status WIFEXITED(STATUS)");
        }
    }

    free(full_path);
    free_tokens(argv_only);
}