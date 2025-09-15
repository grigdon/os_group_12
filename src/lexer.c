#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_input(void) {
	char *buffer = NULL;
	int bufsize = 0;
	char line[5];
	while (fgets(line, 5, stdin) != NULL)
	{
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

void expand_env_tokens(tokenlist *tokens) {
	for(int i = 0; i < tokens->size; i++) {
		// checks for env vars in the form '$USER' and subs for true value
		if(tokens->items[i][0] == '$' && strlen(tokens->items[i]) > 1) {
			char *env_var = getenv(tokens->items[i] + 1);
			if(env_var != NULL) {
				tokens->items[i] = realloc(tokens->items[i], strlen(env_var) + 1);
				strcpy(tokens->items[i], env_var);
			} else { 
				tokens->items[i] = realloc(tokens->items[i], 1);
				tokens->items[i][1] = '\0';
			}
		}
		// checks for tilde expansion 
		if(tokens->items[i][0] == '~' && (tokens->items[i][1] == '\0' || tokens->items[i][1] == '/')) {
			char *home_dir = getenv("HOME");
			if(tokens->items[i][1] == '\0' && home_dir != NULL) {
				tokens->items[i] = realloc(tokens->items[i], strlen(home_dir) + 1);
				strcpy(tokens->items[i], home_dir);
			} else if(tokens->items[i][1] == '/') {
				// store everything after ~/ ... ~/projects/doc_1 => store("projects/doc_1") concatenate this on top of "~" expanded home directory
				char *remaining_dir = tokens->items[i] + 1;
				tokens->items[i] = realloc(tokens->items[i], strlen(home_dir) + strlen(remaining_dir) + 1);
				char *final_dir = strcat(home_dir, remaining_dir);
				strcpy(tokens->items[i], final_dir);
			} else {
				// if that home_dir is == NULL

				tokens->items[i] = realloc(tokens->items[i], 1);
				tokens->items[i] = '\0';
			}
		}
	}
}