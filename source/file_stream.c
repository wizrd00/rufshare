#include "file_stream.h"

static void get_dir(const char *path, char *dir) {
    size_t len = strlen(path) + 1;
    int i;
    strcpy(dir, path);
    for (i = len; i > 0; i--) {
        dir[i] = '\0';
        if (dir[i - 1] == '/')
            break;
    }
    if (i == 1)
        strcpy(dir, "./");
    return;
}

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
    CHECK_INT(fseek(file, size - 1, SEEK_SET), FAILURE);
    fwrite("", sizeof (char), 1, file);
    CHECK_NOTEOF(fflush(file), FAILURE);
    return stat;
}

static status_t create_file(FILE **file, const char *path, size_t size) {
    status_t stat = SUCCESS;
    *file = fopen(path, "w+");
    char dir[strlen(path) + 1];
    get_dir(path, dir);
    CHECK_PTR(*file, FAILURE);
    CHECK_STAT(check_available_space(*file, size));
    CHECK_STAT(expand_file(*file, size));
    return stat;
}

status_t start_file_stream(FileContext* filec, const char *path, bool create) {
    status_t stat = SUCCESS;
    FILE *file;
    struct stat info;
    Buffer tmp_mfile_buf;
    if (create) {
        CHECK_STAT(create_file(&file, path, filec->size));
    }
    else {
        file = fopen(path, "r+");
        CHECK_PTR(file, ECREATE);
    }
    CHECK_INT(fstat(fileno(file), &info), BADARGS);
    tmp_mfile_buf = mmap(NULL, info.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fileno(file), 0);
    CHECK_MMAP(tmp_mfile_buf);
    filec->mfile->file = file;
    filec->mfile->size = info.st_size;
    filec->mfile->pos = 0;
    filec->mfile->buf = tmp_mfile_buf;
    filec->size = info.st_size;
    filec->get_chunk = global_get_chunk;
    return stat;
}

status_t end_file_stream(FileContext *filec) {
    status_t stat = SUCCESS;
    CHECK_NOTEOF(mfclose(filec->mfile), FAILURE);
    CHECK_NOTEOF(fclose(filec->mfile->file), FAILURE);
    return stat;
}

ChunkContext global_get_chunk(unsigned long start_pos, size_t chunk_size) {
    ChunkContext chunk_context = {
        .start_pos = start_pos,
        .chunk_size = chunk_size,
        .cread = global_chunk_read,
        .reset = global_chunk_reset
    };
    return chunk_context;
}

status_t global_chunk_read(MFILE *mfile, size_t *total_size, Buffer buf, size_t len) {
    status_t stat = SUCCESS;
    size_t read_size = (len <= *total_size) ? len : *total_size;
    CHECK_SIZE(mfread(buf, read_size, sizeof (char), mfile), read_size);
    return stat;
}

void global_chunk_reset(MFILE *mfile, unsigned long pos) {
    mfile->pos = pos;
    return;
}
