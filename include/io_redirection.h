#pragma once

typedef struct {
    const char * in_path;
    const char * out_path;
} io_redirection_t;

int apply_io_redirection(const io_redirection_t * r);