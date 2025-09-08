#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include "error/errhandler.h"
#include "logging/logger.h"
#include <stddef.h>
#include <stdio.h>

typedef struct {
    unsigned long start_pos;
    unsigned long piece_count;
    size_t chunk_size;
    size_t piece_size;
    signed int (*get_piece)(Buffer buf);
    signed int (*reset)(void);
} ChunkContext;

typedef struct {
    FILE* file;
    unsigned long chunk_count;
    size_t file_size;
    size_t chunk_size;
    ChunkContext (*get_chunk)(void);
} FileContext;

FileContext start_file_stream(const unsigned char *name);
void end_file_stream(FILE* file);

#endif
