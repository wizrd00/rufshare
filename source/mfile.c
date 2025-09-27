#include "mfile.h"

size_t mfread(void *ptr, size_t size, size_t nmemb, MFILE *stream) {
    size_t tsize = size * nmemb;
    size_t rsize = stream->size - stream->pos;
    size_t fsize = (tsize < rsize) ? tsize : rsize;
    memcpy((unsigned char *) ptr, (unsigned char *) stream->buf, fsize);
    stream->pos += fsize;
    return fsize;
}

size_t mfwrite(const void *ptr, size_t size, size_t nmemb, MFILE *stream) {
    size_t tsize = size * nmemb;
    size_t wsize = stream->size - stream->pos;
    size_t fsize = (tsize < wsize) ? tsize : wsize;
    memcpy((unsigned char *) stream->buf, (unsigned char *) ptr, fsize);
    stream->pos += fsize;
    return fsize;
}

int mfseek(MFILE *stream, long offset) {
    stream->pos += offset;
    return 0;
}

long mftell(MFILE *stream) {
    return (long) stream->pos;
}

int mfclose(MFILE *stream) {
    return munmap(stream->buf, stream->size);
}
