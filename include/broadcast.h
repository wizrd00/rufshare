#ifndef BROADCAST_H
#define BROADCAST_H

#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

status_t push_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t start_broadcast(CntlAddrs *addrs, sockfd_t *sock);

#endif
