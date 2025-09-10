#ifndef TYPES_H
#define TYPES_H

#include "logging/logger.h"

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

typedef unsigned char* Buffer;

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
    if (val != 0)
        except();
    return;
}

#endif
