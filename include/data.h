#ifndef DATA_H
#define DATA_H

#include "types.h"
#include "net_stream.h"
#include "mfile.h"
#include "utils/ipcheck.h"
#include <stdlib.h>
#include <poll.h>

status_t start_data(CntlAddrs *addrs, sockfd_t *sock);

status_t end_data(sockfd_t sock);

status_t push_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout);

status_t pull_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout);

#endif
