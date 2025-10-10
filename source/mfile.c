#include "mfile.h"

MFILE mfopen(const char *pathname, const char *mode, int prot, int flags) {
    MFILE mfile = {.file = NULL, .size = 0, .pos = 0, .buf = NULL};
    FILE *file = fopen(pathname, mode);
    if (file == NULL)
        return mfile;
    mfile.file = file;
    struct stat info;
    if (fstat(fileno(file), &info) == -1)
        return mfile;
    mfile.size = info.st_size;
    void *buf = mmap(NULL, mfile.size, prot, flags, fileno(file), 0);
    if (buf == MAP_FAILED)
        return mfile;
    mfile.buf = buf;
    return mfile;
}

size_t mfread(void *ptr, size_t size, size_t nmemb, MFILE *stream) {
    size_t tsize = size * nmemb;
    size_t rsize = stream->size - stream->pos;
    size_t fsize = (tsize < rsize) ? tsize : rsize;
    memcpy(ptr, stream->buf + stream->pos, fsize);
    stream->pos += fsize;
    return fsize;
}

size_t mfwrite(const void *ptr, size_t size, size_t nmemb, MFILE *stream) {
    size_t tsize = size * nmemb;
    size_t wsize = stream->size - stream->pos;
    size_t fsize = (tsize < wsize) ? tsize : wsize;
    memcpy(stream->buf + stream->pos, ptr, fsize);
    stream->pos += fsize;
    return fsize;
}

int mfseek(MFILE *stream, unsigned long pos) {
    stream->pos = (pos <= stream->size) ? pos : stream->size;
    return 0;
}

long mftell(MFILE *stream) {
    return (long) stream->pos;
}

int mfsync(void *addr, size_t length, int flags) {
    return msync(addr, length, flags);
}

int mfclose(MFILE *stream) {
    int munmap_stat, fclose_stat;
    munmap_stat = (stream->buf != NULL) ? munmap(stream->buf, stream->size) : 0;
    fclose_stat = (stream->file != NULL) ? fclose(stream->file) : 0;
    return munmap_stat | fclose_stat;
}
