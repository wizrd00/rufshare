#ifndef SCANPAIR_H
#define SCANPAIR_H

#include "types.h"
#include "net_stream.h"
#include "utils/infostr.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

status_t start_scanpair(CntlAddrs *addrs, sockfd_t *sock, PairInfo *info, size_t *len, int timeout);

#endif
