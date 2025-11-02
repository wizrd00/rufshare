#ifndef FNAME_H
#define FNAME_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static inline void extract_file_name(char *dst, const char *src, size_t size) {
        char *start = (char *) src;
        for (int i = strlen(src) - 1; i >= 0; i--)
                if (src[i] == '/') {
                        start = (char *) &src[i + 1]; 
                        break;
                }
        strncpy(dst, start, size);
	// TODO
        return;
}

static inline char *make_random_filename(char *filename, size_t size, const char *keyword) {
	snprintf(filename, size, "%s_%d.tmp", keyword, rand());
	return filename;
}

#endif
