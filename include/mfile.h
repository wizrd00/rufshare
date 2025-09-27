#ifndef MFILE_H
#define MFILE_H

#include "types.h"
#include <sys/mman.h>

size_t mfread(void *ptr, size_t size, size_t nmemb, MFILE *stream);

size_t mfwrite(const void *ptr, size_t size, size_t nmemb, MFILE *stream);

int mfseek(MFILE *stream, long offset);

long mftell(MFILE *stream);

int mfclose(MFILE *stream);

#endif
