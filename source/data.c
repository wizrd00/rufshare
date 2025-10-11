#include "data.h"

status_t start_data(CntlAddrs *addrs, sockfd_t *sock) {
    status_t stat = SUCCESS;
    CHECK_BOOL(check_ipv4_format(addrs->local_ip), BADIPV4);
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
    Buffer segbuf = (Buffer) malloc(SEGMENTSIZE);
    struct pollfd pfd = {.fd = sock, .events = POLLOUT};
    CHECK_PTR(segbuf, EMALLOC);
    size_t rsize = chunk.chunk_size;
    mfseek(&(file->mfile), chunk.start_pos);
    CHECK_STAT(set_socket_sndlowsize(sock, 2 * SEGMENTSIZE));
    while (rsize != 0) {
        size_t segsize = (SEGMENTSIZE <= rsize) ? SEGMENTSIZE : rsize;
        mfread(segbuf, segsize, sizeof (char), stream);
        switch (poll(&pfd, 1, timeout)) {
            case -1 :
                return stat = FAILURE;
            case 0 :
                return stat = TIMEOUT;
            default :
                stat = (pfd.revents & POLLOUT) ? SUCCESS : ERRPOLL;
                CHECK_STAT(stat);
                break;
        }
        CHECK_STAT(push_udp_data(sock, segbuf, segsize));
        memset(segbuf, 0, SEGMENTSIZE);
        rsize -= segsize;
    }
    free(segbuf);
    return stat;
}

status_t pull_chunk_data(sockfd_t sock, FileContext *file, ChunkContext chunk, time_t timeout);


