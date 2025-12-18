#ifndef RUFSHARE_NET_STREAM_H
#define RUFSHARE_NET_STREAM_H

#include "types.h"
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BACKLOG 1

status_t init_tcp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port, bool conn);

status_t accept_new_connection(sockfd_t *new_sock, sockfd_t sock, ipv4str_t conn_ip, port_t *conn_port, int timeout);

status_t push_tcp_data(sockfd_t sock, buffer_t buf, size_t size);

status_t pull_tcp_data(sockfd_t sock, buffer_t buf, size_t size, bool peek_flag);

status_t init_udp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port, bool broadcast);

status_t push_udp_data(sockfd_t sock, buffer_t buf, size_t size);

status_t pull_udp_data(sockfd_t sock, buffer_t buf, size_t size);

status_t set_socket_sndlowsize(sockfd_t sock, size_t size);

status_t set_socket_rcvlowsize(sockfd_t sock, size_t size);

status_t set_socket_rcvbufsize(sockfd_t sock, size_t size);

status_t set_socket_sndbufsize(sockfd_t sock, size_t size);

status_t set_socket_timeout(sockfd_t sock, time_t second);

status_t close_socket(sockfd_t sock);

#endif
