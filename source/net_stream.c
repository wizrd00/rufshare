#include "net_stream.h"

static status_t network_byteorder(ipv4str_t ip, uint32_t* dst) {
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

static status_t host_ipstring(ipv4str_t ip, struct sockaddr_in* addr) {
    status_t stat = SUCCESS;
    CHECK_PTR(inet_ntop(addr->sin_family, addr, ip, sizeof (struct sockaddr_in)), BADARGS);
    return stat;
}

status_t init_tcp_socket(sockfd_t* fd, ipv4str_t src_ip, port_t dst_port, ipv4str_t dst_ip, port_t dst_port) {
    status_t stat = SUCCESS;
    uint32_t src_ipnetorder;
    uint32_t src_ipnetorder;
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
    CHECK_INT(*fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), FAILURE);
    CHECK_INT(bind(*fd, &local_addr, sizeof (struct sockaddr_in)), FAILURE);
    CHECK_INT(connect(*fd, &remote_addr, sizeof (struct sockaddr_in)), FAILURE);
    return stat;
}



status_t accept_new_connection(sockfd_t* new_fd, sockfd_t fd, ipv4str_t conn_ip, port_t* conn_port) {
    status_t stat = SUCCESS;
    struct sockaddr_in conn_addr;
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
    CHECK_STAT(host_ipstring(conn_ip, &conn_addr));
    *conn_port = ntohs(conn_addr.sin_port);
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

status_t init_udp_socket(sockfd_t* fd, ipv4str_t dst_ip, port_t dst_port) {
    status_t stat = SUCCESS;
    uint32_t dst_ipnetorder;
    CHECK_STAT(network_byteorder(dst_ip, &dst_ipnetorder));
    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(dst_port),
        .sin_addr = {
            .s_addr = dst_ipnetorder
        }
    };
    CHECK_INT(*fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP), FAILURE);
    CHECK_INT(bind(*fd, &local_addr, sizeof (struct sockaddr_in)), FAILURE);
    return stat;
}

status_t pull_udp_data(sockfd_t fd, ipv4str_t conn_ip, port_t* conn_port, Buffer buf, size_t size) {
    status_t stat = SUCCESS;
    Buffer tmp_buf;
    struct sockaddr_in conn_addr;
    ssize_t received_size = recvfrom(fd, tmp_buf, size, 0, &conn_addr, sizeof (struct sockaddr_in));
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
            default :
                stat = FAILURE;
                return stat;
        }
    else if (received_size < size)
        stat = FAILURE;
    else
        memcpy(buf, tmp_buf, size);
    CHECK_STAT(host_ipstring(conn_ip, &conn_addr));
    *conn_port = ntohs(conn_addr.sin_port);
    return stat;
}

status_t push_udp_data(sockfd_t fd, ipv4str_t dst_ip, port_t dst_port, Buffer buf, size_t size) {
    status_t stat = SUCCESS;
    uint32_t dst_ipnetorder;
    CHECK_STAT(network_byteorder(dst_ip, &dst_ipnetorder));
    struct sockaddr_in remote_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(dst_port),
        .sin_addr = {
            .s_addr = dst_ipnetorder;
        }
    };
    ssize_t sent_size = sendto(fd, buf, size, 0, &remote_addr, sizeof (struct sockaddr_in));
    if (sent_size == -1)
        switch (errno) {
            case EBADF :
            case EFAULT :
            case EINVAL :
            case ENOTSOCK :
                stat = BADARGS;
                return stat;
            case EACCES :
            case EAGAIN :
            case ECONNRESET :
            case EINTR :
            case ENOBUFS :
            case ENOMEM :
            default :
                stat = FAILURE;
                return stat;
        }
    else if (sent_size < size)
        stat = FAILURE;
    return stat;
}

status_t set_socket_rcvbufsize(sockfd_t fd, size_t size) {
    status_t stat = SUCCESS;
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof (size_t)), BADARGS);
    return stat;
}

status_t set_socket_sndbufsize(sockfd_t fd, size_t size) {
    status_t stat = SUCCESS;
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof (size_t)), BADARGS);
    return stat;
}

status_t set_socket_timeout(sockfd_t fd, time_t second) {
    status_t stat = SUCCESS;
    struct timeval timeout = {
        .tv_second = second,
        .tv_usec = 0
    };
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (struct timeval)), BADARGS);
    CHECK_INT(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (struct timeval)), BADARGS);
    return stat;
}

status_t close_socket(sockfd_t fd) {
    status_t stat = SUCCESS;
    if (close(fd) == -1)
        switch (errno) {
            case EBADF :
                stat = BADARGS;
                return stat;
            case EINTR :
            case EIO :
            default :
                stat = FAILURE;
                return stat;
        }
    return stat;
}
