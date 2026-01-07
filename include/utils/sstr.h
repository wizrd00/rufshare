#ifndef RUFSHARE_SSTR_H
#define RUFSHARE_SSTR_H

#include <stddef.h>
#include <string.h>

static inline char *sstrncpy(char *dst, const char *src, size_t dsize)
{
	size_t dlen;
	for (dlen = 0; (dlen < dsize) && (src[dlen] != '\0'); dlen++);
	memcpy(dst, src, dlen);
	dst[(dsize == dlen) ? dsize - 1 : dlen] = '\0';
	return dst;
}
#endif
