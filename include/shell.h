#pragma once

#include "lexer.h"
#include <stdlib.h>

char *search_path(char* token);
void execute_command(tokenlist *tokens);
