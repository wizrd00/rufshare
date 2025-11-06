#ifndef MFILE_H
#define MFILE_H

#include "types.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

MFILE mfopen(const char *pathname, const char *mode, int prot, int flags);

size_t mfread(void *ptr, size_t size, size_t nmemb, MFILE *stream);

size_t mfwrite(const void *ptr, size_t size, size_t nmemb, MFILE *stream);

int mfseek(MFILE *stream, unsigned long pos);

unsigned long mftell(MFILE *stream);

int mfsync(void *addr, size_t length, int flags);

int mfclose(MFILE *stream);

#endif
