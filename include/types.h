#ifndef TYPES_H
#define TYPES_H

#include "logging/logger.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#define CHECK_STAT(val)\
    do {if (val != SUCCESS) {stat = val; return stat;}} while (0)
#define CHECK_INT(val, err)\
    do {if (val == -1) {stat = err; return stat;}} while (0)
#define CHECK_PTR(val, err)\
    do {if (val == NULL) {stat = err; return stat;}} while (0)
#define CHECK_BOOL(val, err)\
    do {if (val == false) {stat = err; return stat;}} while (0)

#ifdef LOG_TRACE
    #define LOGT(mod, pos, msg) logging(&logcount, TRACE, mod, pos, msg)
#else
    #define LOGT(mod, pos, msg)
#endif

#ifdef LOG_DEBUG
    #define LOGD(mod, pos, msg) logging(&logcount, DEBUG, mod, pos, msg)
#else
    #define LOGD(mod, pos, msg)
#endif

#ifdef LOG_WARNN
    #define LOGW(mod, pos, msg) logging(&logcount, WARNN, mod, pos, msg)
#else
    #define LOGW(mod, pos, msg)
#endif

#ifdef LOG_ERROR
    #define LOGE(mod, pos, msg) logging(&logcount, ERROR, mod, pos, msg)
#else
    #define LOGE(mod, pos, msg)
#endif

typedef enum {
    SUCCESS,
    FAILURE,
    TIMEOUT,
    BADARGS
} status_t;

typedef unsigned char* Buffer;

typedef unsigned int sockfd_t;

typedef unsigned char* ipv4str_t;

typedef uint16_t port_t;

typedef struct {
    unsigned long start_pos;
    size_t chunk_size;
    signed int (*cread)(size_t* total_size, Buffer buf, size_t len);
    void (*reset)(unsigned long pos);
} ChunkContext;

typedef struct {
    FILE* file;
    size_t file_size;
    ChunkContext (*get_chunk)(unsigned long start_pos, size_t chunk_size);
} FileContext;

static inline void tryexec(signed int val, void (*except)(void)) {
    if (val != 0 & except != NULL)
        except();
    return;
}

#endif
