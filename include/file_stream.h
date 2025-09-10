#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include "types.h"
#include "error/errors.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

extern FILE* file;

FileContext start_file_stream(const char* path);

void end_file_stream(void);

ChunkContext global_get_chunk(unsigned long start_pos, size_t chunk_size);

signed int global_chunk_read(size_t* total_size, Buffer buf, size_t len);

void global_chunk_reset(unsigned long pos);

#endif
