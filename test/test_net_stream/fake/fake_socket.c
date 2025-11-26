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
	*addrlen = sizeof (struct sockaddr_in);
	return new_sock;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
	if (sockfd == 8) {
		CastPacket pack = pack_RUFShare_CastPacket(0x1337);
		char infostr[320] = "DEMOFILE0:TestMan@192.168.43.81:4096";
		size_t bufsize = sizeof (CastPacket) + 320;
		char *tbuf = malloc(bufsize);
		memcpy(tbuf, (buffer_t) &pack, sizeof (CastPacket));
		strcpy(tbuf + sizeof (CastPacket), infostr);
		memcpy(buf, tbuf + offset, len);
		offset += ((flags & MSG_PEEK) ? 0 : len);
		offset = (bufsize == offset) ? 0 : offset;
		free(tbuf);
		return len;
	}
	else if (sockfd == 64) {
		FlowPacket pack = pack_RUFShare_FlowPacket(1337, 1337, 0x1337);
		size_t bufsize = sizeof (FlowPacket);
		unsigned char *tbuf = malloc(bufsize);
		memcpy(tbuf, (buffer_t) &pack, sizeof (FlowPacket));
		memcpy(buf, tbuf + offset, len);
		offset += ((flags & MSG_PEEK) ? 0 : len);
		offset = (bufsize == offset) ? 0 : offset;
		free(tbuf);
		return len - 1;
	}
	return -1;
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
