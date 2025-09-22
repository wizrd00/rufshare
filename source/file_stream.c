#include "file_stream.h"

status_t start_file_stream(FileContext* filec, const char *path) {
    status_t stat = SUCCESS;
    struct stat info;
    LOGT(__FILE__, __func__, "start opening file...");
    CHECK_PTR(FILE *file = fopen(path, "r+"), FAILURE);
    LOGT(__FILE__, __func__, "file has been opened successfully, calling stat()...");
    CHECK_INT(fstat(fileno(file), &info), BADARGS);
    LOGT(__FILE__, __func__, "func stat() has been called successfully, calling mmap()...");
    CHECK_MMAP(Buffer tmp_mfile_buf = mmap(NULL, info.st_size, PROT_READ | PROT_WRITE, 0, fileno(file), 0));
    FileContext tmp_filec = {
        .mfile = {
            .file = file,
            .pos = 0,
            .buf = tmp_mfile_buf
        },
        .file_size = info.st_size,
        .get_chunk = global_get_chunk
    };
    *filec = tmp_filec;
    LOGT(__FILE__, __func__, "FileContext has been created, returning...");
    return stat;
}

status_t end_file_stream(FileContext *filec) {
    status_t stat = SUCCESS:
    LOGT(__FILE__, __func__, "calling tryexec() for fclose()...");
    CHECK_FCLOSE(filec->mfile->mfclose(filec->mfile));
    return stat;
}

ChunkContext global_get_chunk(unsigned long start_pos, size_t chunk_size) {
    ChunkContext chunk_context = {
        .start_pos = start_pos,
        .chunk_size = chunk_size,
        .cread = global_chunk_read,
        .reset = global_chunk_reset
    };
    LOGT(__FILE__, __func__, "returning chunk_context...");
    return chunk_context;
}

status_t global_chunk_read(FileContext *filec, size_t *total_size, Buffer buf, size_t len) {
    status_t stat = SUCCESS;
    size_t segment_size = (len <= *total_size) ? len : *total_size;
    size_t read_size = segment_size;
    LOGT(__FILE__, __func__, "start copying segment...");
    for (int i = 0; i < 8; i++) {
        size_t offset = segment_size - read_size;
        LOGD(__FILE__, __func__, "calling memcpy()...");
        read_size -= mfread(buf + offset, read_size, sizeof (char), filec->mfile);
        if (read_size == 0) {
            LOGD(__FILE__, __func__, "segment has been copied");
            LOGD(__FILE__, __func__, "*total_size -= segment_size");
            *total_size -= segment_size;
            return stat;
        }
        LOGW(__FILE__, __func__, "fread() couldn't read the exact required size");
    }
    stat = FAILURE;
    LOGE(__FILE__, __func__, "fread() couldn't read the exact required size after 8 retries");
    return stat;
}

void global_chunk_reset(FileContext *filec, unsigned long pos) {
    LOGT(__FILE__, __func__, "resetting file position...");
    filec->mfile->pos = pos;
    return;
}
