#include "fake_socket.h"
#include <string.h>

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
	unsigned char *fbuf = // PI with 100 digit
		"3.141592653589793238462643383279502884197169399375"
		"1058209749445923078164062862089986280348253421170679";
	if (sockfd == 8) {
		memcpy(buf, fbuf, len); // to fake complete recv data
		return len;
	}
	else if (sockfd == 64) {
		memcpy(buf, fbuf, len - 8); // to fake incomplete recv data
		return len - 8;
	}
	return -1; // to fake unsuccessful recv call
}
