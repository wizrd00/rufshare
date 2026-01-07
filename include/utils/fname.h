#ifndef RUFSHARE_FNAME_H
#define RUFSHARE_FNAME_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

static inline char *extract_file_name(const char *src, size_t size)
{
	char *dst = NULL;
	for (int i = (int) size - 1; i >= 0; i--) {
		dst = (char *) src + i;
		if ((i != 0) && (src[i - 1] == '/'))
			break;
	}
        return dst;
}

static inline char *make_random_filename(char *filename, size_t size, const char *keyword)
{
	snprintf(filename, size, "%s_%d.tmp", keyword, rand());
	return filename;
}

#endif
