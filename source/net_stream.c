#include "net_stream.h"

static status_t network_byteorder(ipv4str_t ip, uint32_t *dst) {
    status_t stat = SUCCESS;
    switch (inet_pton(AF_INET, ip, dst)) {
        case -1 :
            stat = BADARGS;
            break;
        case 0 :
            stat = BADIPV4;
            break;
    }
    return stat;
}

static status_t host_ipstring(ipv4str_t ip, struct in_addr *addr) {
    status_t stat = SUCCESS;
    CHECK_PTR(inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN), BADARGS);
    return stat;
}

status_t init_tcp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port) {
    status_t stat = SUCCESS;
    uint32_t src_ipnetorder;
    uint32_t dst_ipnetorder;
    int tmpsock;
    int optval = 1;
    CHECK_STAT(network_byteorder(src_ip, &src_ipnetorder));
    CHECK_STAT(network_byteorder(dst_ip, &dst_ipnetorder));
    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(src_port),
        .sin_addr = {
            .s_addr = src_ipnetorder
        }
    };
    struct sockaddr_in remote_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(dst_port),
        .sin_addr = {
            .s_addr = dst_ipnetorder
        }
    };
    tmpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CHECK_INT(tmpsock, INVSOCK);
    *sock = tmpsock;
    CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET);
    CHECK_INT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND);
    CHECK_INT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN);
    return stat;
}

status_t accept_new_connection(sockfd_t *new_sock, sockfd_t sock, ipv4str_t conn_ip, port_t *conn_port) {
    status_t stat = SUCCESS;
    struct sockaddr_storage conn_addr;
    struct sockaddr_in *tmp_addr;
    socklen_t addr_len;
    int tmpsock;
    CHECK_INT(listen(sock, BACKLOG), FAILURE);
    tmpsock = accept(sock, (struct sockaddr *) &conn_addr, &addr_len);
    CHECK_INT(tmpsock, INVSOCK);
    *new_sock = tmpsock;
    CHECK_EQUAL(AF_INET, conn_addr.ss_family, BADINET);
    tmp_addr = (struct sockaddr_in *) &conn_addr;
    CHECK_STAT(host_ipstring(conn_ip, &(tmp_addr->sin_addr)));
    *conn_port = ntohs(tmp_addr->sin_port);
    return stat;
}

status_t pull_tcp_data(sockfd_t sock, Buffer buf, size_t size, bool peek_flag) {
    status_t stat = SUCCESS;
    ssize_t recv_size = recv(sock, buf, size, MSG_WAITALL | ((peek_flag) ? MSG_PEEK : 0));
    CHECK_INT(recv_size, FAILURE);
    CHECK_SIZE((size_t) recv_size, size);
    return stat;
}

status_t push_tcp_data(sockfd_t sock, Buffer buf, size_t size) {
    status_t stat = SUCCESS;
    ssize_t send_size = send(sock, buf, size, MSG_NOSIGNAL);
    CHECK_INT(send_size, FAILURE);
    CHECK_SIZE((size_t) send_size, size);
    return stat;
}

status_t init_udp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port) {
    status_t stat = SUCCESS;
    uint32_t src_ipnetorder;
    uint32_t dst_ipnetorder;
    int tmpsock;
    int optval = 1;
    CHECK_STAT(network_byteorder(src_ip, &src_ipnetorder));
    CHECK_STAT(network_byteorder(dst_ip, &dst_ipnetorder));
    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(src_port),
        .sin_addr = {
            .s_addr = src_ipnetorder
        }
    };
    struct sockaddr_in remote_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(dst_port),
        .sin_addr = {
            .s_addr = dst_ipnetorder
        }
    };
    tmpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK_INT(tmpsock, INVSOCK);
    *sock = tmpsock;
    CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET);
    CHECK_INT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND);
    CHECK_INT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN);
    return stat;
}

status_t pull_udp_data(sockfd_t sock, Buffer buf, size_t size) {
    status_t stat = SUCCESS;
    ssize_t recv_size = recv(sock, buf, size, 0);
    CHECK_INT(recv_size, FAILURE);
    CHECK_SIZE((size_t) recv_size, size);
    return stat;
}

status_t push_udp_data(sockfd_t sock, Buffer buf, size_t size) {
    status_t stat = SUCCESS;
    ssize_t send_size = send(sock, buf, size, 0);
    CHECK_INT(send_size, FAILURE);
    CHECK_SIZE((size_t) send_size, size);
    return stat;
}

status_t set_socket_rcvbufsize(sockfd_t sock, size_t size) {
    status_t stat = SUCCESS;
    CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof (size_t)), FAILSET);
    return stat;
}

status_t set_socket_sndbufsize(sockfd_t sock, size_t size) {
    status_t stat = SUCCESS;
    CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof (size_t)), FAILSET);
    return stat;
}

status_t set_socket_timeout(sockfd_t sock, time_t second) {
    status_t stat = SUCCESS;
    struct timeval timeout = {
        .tv_sec = second,
        .tv_usec = 0
    };
    CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (struct timeval)), FAILSET);
    CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (struct timeval)), FAILSET);
    return stat;
}

status_t set_socket_broadcast(sockfd_t sock) {
    status_t stat = SUCCESS;
    int optval = 1;
    CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (int)), FAILSET);
    return stat;
}

status_t close_socket(sockfd_t sock) {
    status_t stat = SUCCESS;
    CHECK_INT(close(sock), FAILURE);
    return stat;
}
