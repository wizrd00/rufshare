#include "file_stream.h"

FILE* file;

FileContext start_file_stream(const char* path) {
    FileContext file_context = {
        .file = NULL,
        .file_size = 0,
        .get_chunk = NULL
    };
    LOGT(__FILE__, __func__, "start opening file...");
    file = fopen(path, "r");
    if (file != NULL) {
        struct stat info;
        LOGT(__FILE__, __func__, "file has been opened successfully, calling tryexec() for stat()...");
        tryexec(stat(path, &info), raise_stat_error);
        LOGT(__FILE__, __func__, "func stat() has been called successfully, returning file_context...");
        file_context.file = file;
        file_context.file_size = info.st_size;
        file_context.get_chunk = global_get_chunk;
        return file_context;
    }
    LOGE(__FILE__, __func__, "fopen returned NULL, calling raise_openfile_error()...");
    raise_openfile_error();
    return file_context;
}

void end_file_stream(void) {
    LOGT(__FILE__, __func__, "calling tryexec() for fclose()...");
    tryexec(fclose(file), raise_closefile_error);
    return;
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

signed int global_chunk_read(size_t* total_size, Buffer buf, size_t len) {
    size_t read_size = len;
    LOGT(__FILE__, __func__, "start reading fragment...");
    for (int i = 0; i < 8; i++) {
        size_t offset = len - read_size;
        LOGD(__FILE__, __func__, "calling fread()...");
        read_size -= fread(buf + offset, read_size, sizeof (char), file);
        if (read_size == 0) {
            LOGD(__FILE__, __func__, "fragment has been read");
            LOGD(__FILE__, __func__, "*total_size -= fragment_size");
            *total_size -= len;
            return 0;
        }
        LOGW(__FILE__, __func__, "fread() couldn't read the exact required size");
    }
    LOGE(__FILE__, __func__, "fread() couldn't read the exact required size after 8 retries");
    return -1;
}

void global_chunk_reset(unsigned long pos) {
    LOGT(__FILE__, __func__, "calling tryexec() for fseek()...");
    tryexec(fseek(file, pos, SEEK_SET), raise_seekfile_error);
    return;
}
