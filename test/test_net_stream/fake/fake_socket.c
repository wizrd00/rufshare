#include "fake_socket.h"
#include <string.h>

size_t offset = 0;

int listen(int sockfd, int backlog) {
	return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int new_sock = 512;
	uint32_t ipnetorder;
	struct sockaddr_in *new = (struct sockaddr_in *) addr;
	inet_pton(AF_INET, "1.1.1.1", &ipnetorder);
	struct in_addr ip = {.s_addr = ipnetorder};
	new->sin_family = AF_INET;
	new->sin_port = htons(80);
	new->sin_addr = ip;
	*addrlen = sizeof(struct sockaddr_in);
	return new_sock;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
	if (sockfd == 8)
		return len;
	else if (sockfd == 64)
		return len - 1;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
	if (sockfd == 8)
		return len;
	else if (sockfd == 64)
		return len - 8;
	return -1;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
	fds[0].revents = fds[0].events;
	sleep(timeout / 1000);
	return 1;
}
