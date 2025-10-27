#ifndef CALC_H
#define CALC_H

#include "types.h"
#include "mfile.h"
#include "libcrc/checksum.h"
#include <stddef.h>

static inline RUFShareChunkCount calc_chunk_count(size_t file_size, RUFShareChunkSize chunk_size, RUFSharePartialChunkSize *partial_chunk_size) {
	RUFShareChunkCount val = file_size / chunk_size;
	*partial_chunk_size = file_size - (val * chunk_size);
	return val + 1;
}

static inline RUFShareCRC16 calc_file_crc16(FileContext *filec) {
	return (RUFShareCRC16) crc_16(filec->mfile->buf, filec->size);
}

static inline RUFShareCRC32 calc_chunk_crc32(FileContext *filec, ChunkContext *chunk) {
	return (RUFShareCRC32) crc_32(filec->mfile->buf + chunk->start_pos, chunk->size);
}

#endif
