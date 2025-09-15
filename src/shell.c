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

void execute_command(tokenlist *tokens) {
    int status;
    char *full_path = search_path(tokens->items[0]);

    if(full_path == NULL) {
        perror("command does not exist");
        return; 
    } else {
        pid_t child_pid = fork();
        if(child_pid < 0) { // error state
            perror("fork failed"); 
            return;
        } else if(child_pid == 0) { // child process
            execv(full_path, tokens->items);
            perror("error with executing command");
            exit(EXIT_FAILURE);
            }
        else { 
            waitpid(child_pid, &status, 0); // parent state
            if(!(WIFEXITED(status))) {
                perror("Child terminated with abnormal status WIFEXITED(STATUS)");

            }
        }
        return; 
    }
}