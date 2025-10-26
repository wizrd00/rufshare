#ifndef CALC_H
#define CALC_H

#include "types.h"
#include <stddef.h>

static inline RUFShareChunkCount calc_chunk_count(size_t file_size, RUFShareChunkSize chunk_size, RUFSharePartialChunkSize *partial_chunk_size) {
	RUFShareChunkCount val = file_size / chunk_size;
	*partial_chunk_size = file_size - (val * chunk_size);
	return val + 1;
}

static inline RUFShareCRC16 calc_file_crc16(FileContext *filec);

static inline RUFShareCRC32 calc_file_crc32(void);

#endif
