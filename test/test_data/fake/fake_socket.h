#ifndef FAKE_SOCKET_H
#define FAKE_SOCKET_H

#include "types.h"
#include "utils/infostr.h"
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif
