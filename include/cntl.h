#ifndef CNTL_H
#define CNTL_H

#include "types.h"
#include "net_stream.h"
#include "utils/ipcheck.h"
#include "utils/infostr.h"
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>

status_t start_cntl(CntlAddrs *addrs, sockfd_t *sock, bool conn);

status_t accept_cntl(CntlAddrs *addrs, sockfd_t *new_sock, sockfd_t sock, int timeout);

status_t end_cntl(sockfd_t sock);

status_t push_CAST_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t push_FLOW_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t push_SEND_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t push_RECV_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t pull_CAST_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t pull_FLOW_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t pull_SEND_header(sockfd_t sock, HeaderArgs *args, int timeout);

status_t pull_RECV_header(sockfd_t sock, HeaderArgs *args, int timeout);

#endif
