#include "file_stream.h"

static status_t check_available_space(FILE *file, size_t size) {
    status_t stat = SUCCESS;
    struct statvfs info;
    CHECK_INT(fstatvfs(fileno(file), &info), BADARGS);
    if (info.f_bavail <= size)
        stat = FAILURE;
    return stat;
}

static status_t expand_file(FILE *file, size_t size) {
    status_t stat = SUCCESS;
    CHECK_INT(fseek(file, size, SEEK_SET), FAILURE);
    fwrite("", 1, sizeof (char), file);
    return stat;
}

static status_t create_file(FILE **file, const char *path, size_t size) {
    status_t stat = SUCCESS;
    *file = fopen(path, "r+");
    char dir[strlen(path) + 1];
    get_dir(path, dir);
    CHECK_PTR(*file, FAILURE);
    CHECK_STAT(check_available_space(*file, size));
    CHECK_STAT(expand_file(*file, size));
    return stat;
}

status_t start_file_stream(FileContext* filec, const char *path) {
    status_t stat = SUCCESS;
    FILE *file;
    struct stat info;
    Buffer tmp_mfile_buf;
    LOGT(__FILE__, __func__, "start opening file...");
    CHECK_STAT(create_file(&file, path, filec->size));
    LOGT(__FILE__, __func__, "file has been opened successfully, calling stat()...");
    CHECK_INT(fstat(fileno(file), &info), BADARGS);
    LOGT(__FILE__, __func__, "func stat() has been called successfully, calling mmap()...");
    tmp_mfile_buf = mmap(NULL, info.st_size, PROT_READ | PROT_WRITE, 0, fileno(file), 0);
    CHECK_MMAP(tmp_mfile_buf);
    filec->mfile->file = file;
    filec->mfile->size = info.st_size;
    filec->mfile->pos = 0;
    filec->mfile->buf = tmp_mfile_buf;
    filec->size = info.st_size;
    filec->get_chunk = global_get_chunk;
    LOGT(__FILE__, __func__, "FileContext has been created, returning...");
    return stat;
}

status_t end_file_stream(FileContext *filec) {
    status_t stat = SUCCESS;
    LOGT(__FILE__, __func__, "calling tryexec() for fclose()...");
    CHECK_FCLOSE(fclose(filec->mfile->file));
    CHECK_FCLOSE(mfclose(filec->mfile));
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

status_t global_chunk_read(MFILE *mfile, size_t *total_size, Buffer buf, size_t len) {
    status_t stat = SUCCESS;
    size_t segment_size = (len <= *total_size) ? len : *total_size;
    size_t read_size = segment_size;
    LOGT(__FILE__, __func__, "start copying segment...");
    for (int i = 0; i < 8; i++) {
        size_t offset = segment_size - read_size;
        LOGD(__FILE__, __func__, "calling memcpy()...");
        read_size -= mfread(buf + offset, read_size, sizeof (char), mfile);
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

void global_chunk_reset(MFILE *mfile, unsigned long pos) {
    LOGT(__FILE__, __func__, "resetting file position...");
    mfile->pos = pos;
    return;
}
