#include "helper.h"
#include <stdlib.h>
#include <string.h>

char *str_dup(const char *s) {
    if (s == NULL) {
        return NULL;
    }
    
    const size_t size = strlen(s) + 1;
    char *p = malloc(size);

    if(p != NULL) {
        strcpy(p, s);
    }
    return p;
}