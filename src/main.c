#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "shell.h"
#include "jobs.h"

int main() {
    while (1) {
        check_jobs();

        // print prompt
        char *user_env = getenv("USER");
        char *home_env = getenv("HOME");
        char *machine_env = getenv("MACHINE");
        printf("%s@%s:%s> ", user_env, machine_env, home_env);

        // parse input (lexer)
        char *input = get_input();
        if (input == NULL) {
            printf("\n");
            break;
        }
        tokenlist *tokens = get_tokens(input);

        // execute (shell)
        if (tokens->size > 0) {
            execute_command(tokens);
        }

        // cleanup (lexer)
        free(input);
        free_tokens(tokens);
    }

    return 0;
}