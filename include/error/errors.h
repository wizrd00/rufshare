#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define GLOBAL_ERROR_MSG "[\x1b[1;31mError\x1b[0m]] : \n\t%s\n"

static inline void raise_stat_error(void) {
    fprintf(stderr, GLOBAL_ERROR_MSG, strerror(errno));
    return;
}

static inline void raise_openfile_error(void) {
    fprintf(stderr, GLOBAL_ERROR_MSG, strerror(errno));
    return;
}

static inline void raise_closefile_error(void) {
    fprintf(stderr, GLOBAL_ERROR_MSG, strerror(errno));
}

static inline void raise_seekfile_error(void) {
    fprintf(stderr, GLOBAL_ERROR_MSG, strerror(errno));
}

#endif
