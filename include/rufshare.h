#ifndef RUFSHARE_H
#define RUFSHARE_H

#include "types.h"
#include "file_stream.h"
#include "cntl.h"
#include "data.h"
#include "broadcast.h"
#include "scanpair.h"
#include "tryexec.h"
#include "utils/sigs.h"
#include "utils/calc.h"
#include "utils/fname.h"
#include <string.h>
#include <pthread.h>

status_t push_file(const char *name, const char *path, addr_pair *local, addr_pair *remote, size_t _chunk_size);

status_t pull_file(const char *name, const char *path, addr_pair *local, addr_pair *remote);

status_t scan_pair(PairInfo *info, size_t *len, addr_pair *local);

extern RUFShareChunkSize chunk_size;
extern RUFSharePartialChunkSize partial_chunk_size;
extern RUFShareChunkCount chunk_count;
extern sockfd_t cntl_sock;
extern sockfd_t data_sock;
extern sockfd_t conn_sock;
extern sockfd_t cast_sock;
extern FileContext filec;
extern CntlAddrs addrs;

#endif
