#ifndef BROADCAST_H
#define BROADCAST_H

#include "types.h"
#include <sys/socket.h>

status_t start_broadcast(CntlAddrs *addrs, sockfd_t *sock);

#endif
