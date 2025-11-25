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
	char text[] = 
		"2.7182818284590452353602874713526624977572470936999595749669676277240766303"
		"5354759457138217852516642742746639193200305992181741359662904357290033429526"
		"05956307381323286279434907632338298807531952510190115738341879307021540891499"
		"34884167509244761460668082264800168477411853742345442437107539077744992069";
	memcpy(buf, text, len);
	return len;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
	return len;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
	fds[0].revents = fds[0].events;
	sleep(timeout / 1000);
	return 1;
}
