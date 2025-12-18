#ifndef RUFSHARE_SSTR_H
#define RUFSHARE_SSTR_H

#include <stddef.h>
#include <string.h>

static inline char *sstrncpy(char *dst, const char *src, size_t dsize)
{
	strncpy(dst, src, dsize - 1);
		dst[dsize - 1] = '\0';
	return dst;
}

#endif
