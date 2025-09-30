#include "io_redirection.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static int check_if_regular(const char * path) {
    struct stat st;
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) {
        return 0;
    }

    return 1; 
}

int apply_io_redirection(const io_redirection_t * r) {
    if (!r) return 0;

    if (r->in_path) {
        if (!check_if_regular(r->in_path)) {
            fprintf(stderr, "Error: '%s' is either not a regular file or doesn't exist: %s\n", r->in_path, strerror(errno));
            return -1;
        }

        int in_file_descriptor = open(r->in_path, O_RDONLY);
        if (in_file_descriptor < 0) {
            fprintf(stderr, "Error: Couldn't open input file '%s': %s\n", r->in_path, strerror(errno));
            return -1;
        }

        if (dup2(in_file_descriptor, STDIN_FILENO) < 0) {
            fprintf(stderr, "Error: Couldn't redirect stdin to '%s': %s\n", r->in_path, strerror(errno));
            close(in_file_descriptor);
            return -1;
        }

        close(in_file_descriptor);
    }

    if (r->out_path) {
        int out_file_descripter = open(r->out_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (out_file_descripter < 0) {
            fprintf(stderr, "Error: Couldn't open output file '%s': %s\n", r->out_path, strerror(errno));
            return -1;
        }

        if (dup2(out_file_descripter, STDOUT_FILENO) < 0) {
            fprintf(stderr, "Error: Couldn't redirect stdout to '%s': %s\n", r->out_path, strerror(errno));
            close(out_file_descripter);
            return -1;
        }

        close(out_file_descripter);
    }

    return 0;
}