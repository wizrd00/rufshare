#ifndef FNAME_H
#define FNAME_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static inline char *extract_file_name(char *dst, const char *src, size_t size)
{
	if (src[size - 1] != '\0')
		dst = NULL;
	else
		for (int i = (int) size - 1; i >= 0; i--)
			if (src[i] == '/')
				return dst = src + i + 1;
			else if (i == 0)
				return dst = src;
        return dst;
}

static inline char *make_random_filename(char *filename, size_t size, const char *keyword)
{
	snprintf(filename, size, "%s_%d.tmp", keyword, rand());
	return filename;
}

#endif
