#ifndef DATA_H
#define DATA_H

#include "types.h"
#include "net_stream.h"
#include "mfile.h"
#include "utils/ipcheck.h"
#include <time.h>
#include <poll.h>

status_t start_data(CntlAddrs *addrs, sockfd_t *sock);

status_t end_data(sockfd_t sock);

status_t push_chunk_data(sockfd_t sock, FileContext *file, ChunkContext chunk, time_t timeout);

status_t pull_chunk_data(sockfd_t sock, FileContext *file, ChunkContext chunk, time_t timeout);

#endif
