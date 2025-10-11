#include "rufshare.h"

static status_t handshake(void);

status_t push_file(const char *name, const char *path, addr_pair *local, addr_pair *remote) {
    FileContext filec;
    CntlAddrs cntl_addr = {
        .local_ip = local->ip,
        .local_port = local->port,
        .remote_ip = remote->ip,
        .remote_port = remote->port
        };
    sockfd_t cntl_sock;
    SendPacket spacket;
    RecvPacket rpacket;
    RUFShareChunkSize chunk_size;
    RUFSharePartialChunkSize partial_chunk_size;
    RUFShareChunkCount chunk_count;
    tryexec(start_file_stream(&filec, path, MRD), raise_start_file_stream_error, path);
    tryexec(start_cntl(&cntl_addr, &cntl_sock), raise_start_cntl_error , local);
    chunk_size = calc_chunk_size(filec.size);
    partial_chunk_size = calc_partial_chunk_size(filec.size, chunk_size);
    chunk_count = calc_chunk_count(filec.size, chunk_size);
    // TODO
}

status_t pull_file(const char *path, addr_pair *local, addr_pair *remote);

status_t scan_pair(PairInfo *info, addr_pair *local);
