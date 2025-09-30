#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_input(void) {
    char *buffer = NULL;
    int bufsize = 0;
    char line[5];
    int read_any = 0;
    while (fgets(line, 5, stdin) != NULL)
    {
       read_any = 1;
       int addby = 0;
       char *newln = strchr(line, '\n');
       if (newln != NULL)
          addby = newln - line;
       else
          addby = 5 - 1;
       buffer = (char *)realloc(buffer, bufsize + addby);
       memcpy(&buffer[bufsize], line, addby);
       bufsize += addby;
       if (newln != NULL)
          break;
    }
    if (!read_any) {
       return NULL;
    }
    buffer = (char *)realloc(buffer, bufsize + 1);
    buffer[bufsize] = 0;
    return buffer;
}

tokenlist *new_tokenlist(void) {
    tokenlist *tokens = (tokenlist *)malloc(sizeof(tokenlist));
    tokens->size = 0;
    tokens->items = (char **)malloc(sizeof(char *));
    tokens->items[0] = NULL; /* make NULL terminated */
    return tokens;
}

void add_token(tokenlist *tokens, char *item) {
    int i = tokens->size;

    tokens->items = (char **)realloc(tokens->items, (i + 2) * sizeof(char *));
    tokens->items[i] = (char *)malloc(strlen(item) + 1);
    tokens->items[i + 1] = NULL;
    strcpy(tokens->items[i], item);

    tokens->size += 1;
}

tokenlist *get_tokens(char *input) {
    char *buf = (char *)malloc(strlen(input) + 1);
    strcpy(buf, input);
    tokenlist *tokens = new_tokenlist();
    char *tok = strtok(buf, " ");
    while (tok != NULL)
    {
       add_token(tokens, tok);
       tok = strtok(NULL, " ");
    }
    expand_env_tokens(tokens);
    free(buf);
    return tokens;
}

void free_tokens(tokenlist *tokens) {
    for (int i = 0; i < tokens->size; i++)
       free(tokens->items[i]);
    free(tokens->items);
    free(tokens);
}

void expand_env_tokens(const tokenlist *tokens) {
    if (tokens->items == NULL ) {
       return; // we're cooked
    }
    for(int i = 0; i < tokens->size; i++) {
       // checks for env vars in the form '$USER' and subs for true value
       if(tokens->items[i][0] == '$' && strlen(tokens->items[i]) > 1) {
          const char *env_var = getenv(tokens->items[i] + 1);
          if(env_var != NULL) {
             free(tokens->items[i]);
             tokens->items[i] = malloc(strlen(env_var) + 1); // changed this to be a little more readable
             strcpy(tokens->items[i], env_var);
          } else {
             free(tokens->items[i]);
             tokens->items[i] = malloc(1);
             tokens->items[i][0] = '\0';
          }
       }
       // checks for tilde expansion
       if(tokens->items[i][0] == '~' && (tokens->items[i][1] == '\0' || tokens->items[i][1] == '/')) {
          const char *home_dir = getenv("HOME");
          if(home_dir != NULL && tokens->items[i][1] == '\0') {
             free(tokens->items[i]);
             tokens->items[i] = malloc(strlen(home_dir) + 1);
             strcpy(tokens->items[i], home_dir);
          } else if(home_dir != NULL && tokens->items[i][1] == '/') {
             // store everything after ~/ ... ~/projects/doc_1 => store("projects/doc_1") concatenate this on top of "~" expanded home directory
             const char *remaining_dir = tokens->items[i] + 1;
             char *new_path = malloc(strlen(home_dir) + strlen(remaining_dir) + 1);
             strcpy(new_path, home_dir);
             strcat(new_path, remaining_dir);
             free(tokens->items[i]);
             tokens->items[i] = new_path;
          } else {
             // if that home_dir is == NULL
             free(tokens->items[i]);
             tokens->items[i] = malloc(1);
             tokens->items[i][0] = '\0';
          }
       }
    }
}