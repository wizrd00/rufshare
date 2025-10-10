#include "data.h"

status_t start_data(CntlAddrs *addrs, sockfd_t *sock) {
    status_t stat = SUCCESS;
    CHECK_BOOL(check_ipv4_format(addrs->local_ip), BADIPV4);
    CHECK_BOOL(check_ipv4_format(addrs->remote_ip), BADIPV4);
    CHECK_PORT(addrs->local_port);
    CHECK_PORT(addrs->remote_port);
    CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port);
    return stat;
}

status_t end_data(sockfd_t sock) {
    status_t stat = SUCCESS;
    CHECK_INT(close(sock), FAILURE);
    return stat;
}

status_t push_chunk_data(sockfd_t sock, FileContext *file, ChunkContext chunk, time_t timeout) {
    status_t stat = SUCCESS;
    MFILE *stream = &(file->mfile);
    Buffer seg = (Buffer) malloc(SEGMENTSIZE);
    CHECK_PTR(seg, EMALLOC);
    size_t rsize = chunk.chunk_size;
    mfseek(&(file->mfile), chunk.start_pos);
    while (rsize != 0) {
        size_t segsize = (SEGMENTSIZE <= rsize) ? SEGMENTSIZE : rsize;
        mfread(segment, segsize, sizeof (char), stream);
        // TODO
    }
    free(seg);
}

status_t pull_chunk_data(sockfd_t sock, FileContext *file, ChunkContext chunk, time_t timeout);


