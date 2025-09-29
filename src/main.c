#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lexer.h"
#include "shell.h"
#include "jobs.h"

int main() {
    while (1) {
        check_jobs();

        // print prompt
        char *user_env = getenv("USER");
        char *machine_env = getenv("MACHINE");
        char *current_dir = getcwd(NULL, 0); // POSIX by default doesn't update $PWD on cd changes. It does update cwd though.

        printf("%s@%s:%s> ", user_env, machine_env, current_dir);
        free(current_dir);
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