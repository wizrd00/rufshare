#include "fake_socket.h"

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
