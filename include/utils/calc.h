#ifndef RUFSHARE_CALC_H
#define RUFSHARE_CALC_H

#include "types.h"
#include "mfile.h"
#include "libcrc/checksum.h"
#include <stddef.h>

#define DEFAULT0_SEGMENT_SIZE 1024
#define DEFAULT1_SEGMENT_SIZE 2048

static inline RUFShareChunkCount calc_chunk_count(size_t file_size, size_t chunk_size, RUFSharePartialChunkSize *partial_chunk_size)
{
	RUFShareChunkCount val = (RUFShareChunkCount) (file_size / chunk_size);
	*partial_chunk_size = (RUFSharePartialChunkSize) (file_size - (val * chunk_size));
	return val + 1;
}

static inline size_t calc_file_size(RUFShareChunkCount chunk_count, RUFShareChunkSize chunk_size, RUFSharePartialChunkSize partial_chunk_size)
{
	return (size_t) (((size_t) chunk_count - 1) * (size_t) chunk_size + (size_t) partial_chunk_size);
}

static inline size_t calc_segment_size(size_t size)
{
	if (size > DEFAULT1_SEGMENT_SIZE)
		return DEFAULT1_SEGMENT_SIZE;
	else if (size > DEFAULT0_SEGMENT_SIZE)
		return DEFAULT0_SEGMENT_SIZE;
	else
		return size;
}

static inline RUFShareCRC16 calc_file_crc16(FileContext *filec)
{
	return (RUFShareCRC16) crc_16((const unsigned char *) filec->mfile.buf, filec->size);
}

static inline RUFShareCRC32 calc_chunk_crc32(FileContext *filec, ChunkContext *chunk)
{
	return (RUFShareCRC32) crc_32((const unsigned char *) ((char *) filec->mfile.buf + chunk->start_pos), chunk->chunk_size);
}

#endif
