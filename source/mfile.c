#include "mfile.h"

MFILE mfopen(const char *pathname, const char *mode, int prot, int flags)
{
	MFILE mfile = {.file = NULL, .open = 0, .size = 0, .pos = 0, .buf = NULL};
	struct stat info;
	FILE *file = fopen(pathname, mode);
	if (file == NULL)
		return mfile;
	if (fstat(fileno(file), &info) == -1) {
		fclose(file);
		return mfile;
	}
	mfile.size = info.st_size;
	void *buf = mmap(NULL, mfile.size, prot, flags, fileno(file), 0);
	if (buf == MAP_FAILED) {
		fclose(file);
		return mfile;
	}
	mfile.buf = buf;
	mfile.file = file;
	mfile.open = 1;
	return mfile;
}

size_t mfread(void *ptr, size_t size, size_t nmemb, MFILE *stream)
{
	size_t value = 0;
	if (!stream->open)
		return value;
	size_t tsize = size * nmemb;
	size_t rsize = stream->size - stream->pos;
	size_t fsize = (tsize < rsize) ? tsize : rsize;
	memcpy(ptr, stream->buf + stream->pos, fsize);
	stream->pos += fsize;
	value = fsize;
	return value;
}

size_t mfwrite(const void *ptr, size_t size, size_t nmemb, MFILE *stream)
{
	size_t value = 0;
	if (!stream->open)
		return value;
	size_t tsize = size * nmemb;
	size_t wsize = stream->size - stream->pos;
	size_t fsize = (tsize < wsize) ? tsize : wsize;
	memcpy(stream->buf + stream->pos, ptr, fsize);
	stream->pos += fsize;
	value = fsize;
	return value;
}

int mfseek(MFILE *stream, unsigned long pos)
{
	int value = -1;
	if (!stream->open)
		return value;
	stream->pos = (pos <= stream->size) ? pos : stream->size;
	value = 0;
	return value;
}

unsigned long mftell(MFILE *stream)
{
	unsigned long value = -1;
	if (!stream->open)
		return value;
	return (unsigned long) stream->pos;
}

int mfsync(void *addr, size_t length, int flags)
{
	return msync(addr, length, flags);
}

int mfclose(MFILE *stream)
{
	int munmap_stat, fclose_stat;
	munmap_stat = (stream->buf != NULL) ? munmap(stream->buf, stream->size) : 0;
	fclose_stat = (stream->file != NULL) ? fclose(stream->file) : 0;
	return munmap_stat | fclose_stat;
}
