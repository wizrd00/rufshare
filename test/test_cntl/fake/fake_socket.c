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
		char infostr[INFOSTRSIZE] = "DEMOFILE0:TestMan@192.168.43.81:4096";
		size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
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
		return len;
	}
	else if (sockfd == 512) {
		SendPacket pack = pack_RUFShare_SendPacket(1337, 1337, 2, 0x1337);
		char infostr[INFOSTRSIZE] = "DEMOFILE1:TestMan@192.168.43.81:4096";
		size_t bufsize = sizeof (SendPacket) + INFOSTRSIZE;
		char *tbuf = malloc(bufsize);
		memcpy(tbuf, (buffer_t) &pack, sizeof (SendPacket));
		strcpy(tbuf + sizeof (SendPacket), infostr);
		memcpy(buf, tbuf + offset, len);
		offset += ((flags & MSG_PEEK) ? 0 : len);
		offset = (bufsize == offset) ? 0 : offset;
		free(tbuf);
		return len;
	}
	else if (sockfd == 4096) {
		RecvPacket pack = pack_RUFShare_RecvPacket(1, 0x1337, 1337);
		size_t bufsize = sizeof (RecvPacket);
		unsigned char *tbuf = malloc(bufsize);
		memcpy(tbuf, (buffer_t) &pack, sizeof (RecvPacket));
		memcpy(buf, tbuf + offset, len);
		offset += ((flags & MSG_PEEK) ? 0 : len);
		offset = (bufsize == offset) ? 0 : offset;
		free(tbuf);
		return len;
	}
	return -1;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
	return len;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
	fds[0].revents = fds[0].events;
	sleep(timeout / 1000);
	return 1;
}
