#include "cntl.h"

status_t start_cntl(CntlAddrs *addr, sockfd_t *sock) {
    status_t stat = SUCCESS;
    CHECK_IPV4(addr->local_ip);
    CHECK_IPV4(addr->remote_ip);
    CHECK_PORT(addr->local_port);
    CHECK_PORT(addr->remote_port);
    CHECK_STAT(init_tcp_socket(sock, addr->local_ip, addr->local_port, addr->remote_ip, addr->remote_port));
    return stat;
}

status_t end_cntl(sockfd_t sock) {
    status_t stat = SUCCESS;
    CHECK_INT(close(sock), FAILURE);
    return stat;
}

status_t push_CAST_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    char infostr[INFOSTRSIZE];
    size_t bufsize = sizeof(CastPacket) + INFOSTRSIZE;
    Buffer buf = (Buffer) malloc(bufsize);
    struct pollfd pfd = {.fd = sock, .events = POLLOUT};
    CHECK_PTR(buf, EMALLOC);
    pack_into_infostring(infostr, &(args->cast.info));
    memcpy(buf, &(args->cast.packet), sizeof (CastPacket));
    memcpy(buf + sizeof (CastPacket), infostr, sizeof (infostr));
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            stat = FAILURE;
            break;
        case 0 :
            stat = TIMEOUT;
            break;
        default :
            stat = (pfd.revents > 0) ? push_tcp_data(sock, buf, bufsize) : FAILURE;
            break;
    }
    free(buf);
    return stat;
}

status_t push_FLOW_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    size_t bufsize = sizeof(FlowPacket);
    Buffer buf = (Buffer) malloc(bufsize);
    struct pollfd pfd = {.fd = sock, .events = POLLOUT};
    CHECK_PTR(buf, EMALLOC);
    memcpy(buf, &(args->flow.packet), sizeof (FlowPacket));
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            stat = FAILURE;
            break;
        case 0 :
            stat = TIMEOUT;
            break;
        default :
            stat = (pfd.revents > 0) ? push_tcp_data(sock, buf, bufsize) : FAILURE;
            break;
    }
    free(buf);
    return stat;
}

status_t push_SEND_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    char infostr[INFOSTRSIZE];
    size_t bufsize = sizeof(SendPacket) + INFOSTRSIZE;
    Buffer buf = (Buffer) malloc(bufsize);
    struct pollfd pfd = {.fd = sock, .events = POLLOUT};
    CHECK_PTR(buf, EMALLOC);
    pack_into_infostring(infostr, &(args->send.info));
    memcpy(buf, &(args->send.packet), sizeof (SendPacket));
    memcpy(buf + sizeof (SendPacket), infostr, sizeof (infostr));
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            stat = FAILURE;
            break;
        case 0 :
            stat = TIMEOUT;
            break;
        default :
            stat = (pfd.revents > 0) ? push_tcp_data(sock, buf, bufsize) : FAILURE;
            break;
    }
    free(buf);
    return stat;
}

status_t push_RECV_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    size_t bufsize = sizeof(RecvPacket);
    Buffer buf = (Buffer) malloc(bufsize);
    struct pollfd pfd = {.fd = sock, .events = POLLOUT};
    CHECK_PTR(buf, EMALLOC);
    memcpy(buf, &(args->recv.packet), sizeof (RecvPacket));
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            stat = FAILURE;
            break;
        case 0 :
            stat = TIMEOUT;
            break;
        default :
            stat = (pfd.revents > 0) ? push_tcp_data(sock, buf, bufsize) : FAILURE;
            break;
    }
    free(buf);
    return stat;
}

status_t pull_CAST_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    sockfd_t conn_sock;
    CastPacket packet;
    char infostr[INFOSTRSIZE];
    RUFShareType tmp_type;
    struct pollfd pfd = {.fd = sock, .events = POLLIN};
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            return stat = FAILURE;
        case 0 :
            return stat = TIMEOUT;
        default :
            stat = (pfd.revents > 0) ? accept_new_connection(&conn_sock, sock, args->cast.info.remote_ip, &(args->cast.info.remote_port)) : FAILURE;
            CHECK_STAT(stat);
            break;
    }
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &tmp_type, sizeof (RUFShareType), true));
    if (ntohs(tmp_type) != CAST)
        return stat = BADTYPE;
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &packet, sizeof (CastPacket), false));
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) infostr, sizeof (infostr), false));
    args->cast.packet = convert_CastPacket_byteorder(&packet);
    args->cast.info = unpack_from_infostring(infostr);
    return stat;
}

status_t pull_FLOW_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    sockfd_t conn_sock;
    FlowPacket packet;
    RUFShareType tmp_type;
    struct pollfd pfd = {.fd = sock, .events = POLLIN};
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            return stat = FAILURE;
        case 0 :
            return stat = TIMEOUT;
        default :
            stat = (pfd.revents > 0) ? accept_new_connection(&conn_sock, sock, args->flow.info.remote_ip, &(args->flow.info.remote_port)) : FAILURE;
            CHECK_STAT(stat);
            break;
    }
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &tmp_type, sizeof (RUFShareType), true));
    if (ntohs(tmp_type) != FLOW)
        return stat = BADTYPE;
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &packet, sizeof (FlowPacket), false));
    args->flow.packet = convert_FlowPacket_byteorder(&packet);
    return stat;
}

status_t pull_SEND_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    sockfd_t conn_sock;
    SendPacket packet;
    char infostr[INFOSTRSIZE];
    RUFShareType tmp_type;
    struct pollfd pfd = {.fd = sock, .events = POLLIN};
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            return stat = FAILURE;
        case 0 :
            return stat = TIMEOUT;
        default :
            stat = (pfd.revents > 0) ? accept_new_connection(&conn_sock, sock, args->send.info.remote_ip, &(args->send.info.remote_port)) : FAILURE;
            CHECK_STAT(stat);
            break;
    }
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &tmp_type, sizeof (RUFShareType), true));
    if (ntohs(tmp_type) != SEND)
        return stat = BADTYPE;
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &packet, sizeof (SendPacket), false));
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) infostr, sizeof (infostr), false));
    args->send.packet = convert_SendPacket_byteorder(&packet);
    args->send.info = unpack_from_infostring(infostr);
    return stat;
}

status_t pull_RECV_header(sockfd_t sock, HeaderArgs *args, time_t timeout) {
    status_t stat = SUCCESS;
    sockfd_t conn_sock;
    RecvPacket packet;
    RUFShareType tmp_type;
    struct pollfd pfd = {.fd = sock, .events = POLLIN};
    switch (poll(&pfd, 1, timeout)) {
        case -1 :
            return stat = FAILURE;
        case 0 :
            return stat = TIMEOUT;
        default :
            stat = (pfd.revents > 0) ? accept_new_connection(&conn_sock, sock, args->recv.info.remote_ip, &(args->recv.info.remote_port)) : FAILURE;
            CHECK_STAT(stat);
            break;
    }
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &tmp_type, sizeof (RUFShareType), true));
    if (ntohs(tmp_type) != RECV)
        return stat = BADTYPE;
    CHECK_STAT(pull_tcp_data(conn_sock, (Buffer) &packet, sizeof (RecvPacket), false));
    args->recv.packet = convert_RecvPacket_byteorder(&packet);
    return stat;
}
