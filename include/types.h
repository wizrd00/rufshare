#ifndef TYPES_H
#define TYPES_H

#include "logging/logger.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
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
#define CHECK_FCLOSE(val)\
    do {if (val == EOF) {stat = FAILURE; return stat;}} while (0)
#define CHECK_MMAP(val)\
    do {if (val == MAP_FAILED) {stat = FAILURE; return stat;}} while (0)

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
    BADARGS,
    EMALLOC
} status_t;

typedef unsigned char* Buffer;

typedef unsigned int sockfd_t;

typedef unsigned char* ipv4str_t;

typedef uint16_t port_t;

typedef struct {
} local_info;

typedef struct {
    FILE* file;
    size_t pos;
    Buffer buf;
} MFILE;

typedef struct {
    unsigned long start_pos;
    size_t chunk_size;
    status_t (*cread)(FileContext* filec, size_t* total_size, Buffer buf, size_t len);
    void (*reset)(FileContext* filec, unsigned long pos);
} ChunkContext;

typedef struct {
    MFILE* mfile;
    size_t file_size;
    ChunkContext (*get_chunk)(unsigned long start_pos, size_t chunk_size);
} FileContext;

#endif
