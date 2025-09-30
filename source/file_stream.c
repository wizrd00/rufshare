#include "file_stream.h"

static status_t create_file(const char *path, size_t size) {
    status_t stat = SUCCESS;
    struct statvfs info;
    FILE *file;
    CHECK_NOTEQUAL(0, size, BADARGS);
    CHECK_INT(statvfs(path, &info), NOFSTAT);
    if (info.f_bavail * info.f_bsize <= size)
        return stat = NOAVAIL;
    file = fopen(path, "w");
    CHECK_PTR(file, NOCREAT);
    CHECK_INT(ftruncate(fileno(file), size), NOTRUNC);
    return stat;
}

status_t start_file_stream(FileContext* filec, const char *path, fmode_t mode) {
    status_t stat = SUCCESS;
    MFILE mfile;
    if (mode == MWR)
        create_file(path, filec->size);
    mfile = mfopen(path, "r+", PROT_READ | PROT_WRITE, MAP_SHARED);
    CHECK_MFILE(mfile);
    filec->mfile = mfile;
    return stat;
}

status_t end_file_stream(FileContext *filec) {
    status_t stat = SUCCESS;
    CHECK_EQUAL(0, mfclose(&(filec->mfile)), FAILURE);
    return stat;
}

ChunkContext global_get_chunk(unsigned long start_pos, size_t chunk_size) {
    ChunkContext chunk_context = {
        .start_pos = start_pos,
        .chunk_size = chunk_size
    };
    return chunk_context;
}
