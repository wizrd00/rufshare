#ifndef RUFSHARE_MFILE_H
#define RUFSHARE_MFILE_H

#include "types.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

MFILE mfopen(const char *pathname, const char *mode, int prot, int flags);

size_t mfread(void *ptr, size_t size, size_t nmemb, MFILE *stream);

size_t mfwrite(const void *ptr, size_t size, size_t nmemb, MFILE *stream);

size_t mfseek(MFILE *stream, size_t pos);

size_t mftell(MFILE *stream);

int mfsync(MFILE *stream, int flags);

int mfclose(MFILE *stream);

#endif
