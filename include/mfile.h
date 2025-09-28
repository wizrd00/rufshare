#ifndef MFILE_H
#define MFILE_H

#include "types.h"
#include <sys/mman.h>

MFILE mfopen(const char *pathname, const char *mode, size_t size, int prot, int flags);

size_t mfread(void *ptr, size_t size, size_t nmemb, MFILE *stream);

size_t mfwrite(const void *ptr, size_t size, size_t nmemb, MFILE *stream);

int mfseek(MFILE *stream, long offset);

long mftell(MFILE *stream);

int mfsync(void *addr, size_t length, int flags);

int mfclose(MFILE *stream);

#endif
