#ifndef ERROR_H
#define ERROR_H

#include "types.h"
#include <stdio.h>

static inline void raise_start_file_stream_error(void *arg);

static inline void raise_start_cntl_error(void *arg);

static inline void tryexec(status_t stat, void (*except)(void *arg), void *arg) {
	if (stat != SUCCESS)
		except(arg);
		return;
}

#endif
