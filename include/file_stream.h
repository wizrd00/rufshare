#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include "types.h"
#include "mfile.h"
#include "error/errors.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

status_t start_file_stream(FileContext* filec, const char *path, bool create);

status_t end_file_stream(FileContext* filec);

ChunkContext global_get_chunk(unsigned long start_pos, size_t chunk_size);

status_t global_chunk_read(MFILE *mfile, size_t *total_size, Buffer buf, size_t len);

void global_chunk_reset(MFILE *mfile, unsigned long pos);

#endif
