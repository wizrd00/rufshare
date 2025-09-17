#include "net_stream.h"

static status_t convert_ipstring_to_network_byteorder(ipv4str_t ip, uint32_t* dst) {
    status_t stat;
    switch (inet_pton(AF_INET, ip, dst)) {
        case -1 :
            stat = FAILURE;
            return stat;
        case 0 :
            stat = BADARGS;
            return stat;
        case 1 :
            stat = SUCCESS;
            return stat;
    }
    return stat;
}

status_t init_tcp_socket(sockfd_t* fd, ipv4str_t ip, port_t port) {
    status_t stat = SUCCESS;
    uint32_t ipnetorder;
    CHECK_STAT(convert_ipstring_to_network_byteorder(ip, &ipnetorder));
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {
            .s_addr = ipnetorder
        }
    };
    CHECK_INT(*fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), FAILURE);
    CHECK_INT(connect(*fd, &addr, sizeof (struct sockaddr_in)), FAILURE);
    CHECK_INT(bind(*fd, &addr, sizeof (struct sockaddr_in)), FAILURE);
    return stat;
}

status_t set_tcp_socket_rcvbufsize(sockfd_t fd, size_t size) {
    status_t stat = SUCCESS;
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof (size_t)), BADARGS);
    return stat;
}

status_t set_tcp_socket_sndbufsize(sockfd_t fd, size_t size) {
    status_t stat = SUCCESS;
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof (size_t)), BADARGS);
    return stat;
}

status_t set_tcp_socket_timeout(sockfd_t fd, time_t second) {
    status_t stat = SUCCESS;
    struct timeval timeout = {
        .tv_second = second,
        .tv_usec = 0
    };
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (struct timeval)), BADARGS);
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (struct timeval)), BADARGS);
    return stat;
}

status_t accept_new_connection(sockfd_t* new_fd, sockfd_t fd, struct sockaddr_in* addr) {
    status_t stat = SUCCESS;
    if (listen(fd, BACKLOG) == -1)
        switch (errno) {
            case EBADF :
            case ENOTSOCK :
                stat = BADARGS;
                return stat;
            case EADDRINUSE :
            case EOPNOTSUPP :
            default :
                stat = FAILURE;
                return stat;

        }
    CHECK_INT(*new_fd = accept(fd, addr, sizeof (sockaddr_in)), FAILURE);
    return stat;
}

status_t pull_tcp_data(sockfd_t fd, Buffer buf, size_t size) {
    status_t stat = SUCCESS;
    Buffer tmp_buf[size];
    ssize_t received_size = recv(fd, tmp_buf, size, MSG_WAITALL);
    if (received_size == -1)
        switch (errno) {
            case EBADF :
            case EINVAL :
            case ENOTSOCK :
                stat = BADARGS;
                return stat;
            case EAGAIN :
            case ECONNREFUSED :
            case EFAULT :
            case EINTR :
            case ENOMEM :
            case ENOTCONN :
            default :
                stat = FAILURE;
                return stat;
        }
    else if (received_size < size)
        stat = FAILURE;
    else
        memcpy(buf, tmp_buf, size);
    return stat;
}

status_t push_tcp_data(sockfd_t fd, Buffer buf, size_t size) {
    status_t stat = SUCCESS;
    ssize_t sent_size = send(fd, buf, size, MSG_NOSIGNAL);
    if (sent_size == -1)
        switch (errno) {
            case EBADF :
            case EFAULT :
            case EINVAL :
            case ENOTSOCK :
            case EOPNOTSUPP :
                stat = BADARGS;
                return stat;
            case EACCES :
            case EAGAIN :
            case ECONNRESET :
            case EDESTADDRREQ :
            case EINTR :
            case ENOBUFS :
            case ENOTCONN :
            default :
                stat = FAILURE;
                return stat;
        }
    else if (sent_size < size)
        stat = FAILURE;
    return stat;
}
