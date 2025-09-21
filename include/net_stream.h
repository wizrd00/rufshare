#ifndef NET_STREAM_H
#define NET_STREAM_H

#include "types.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define BACKLOG 1

status_t init_tcp_socket(sockfd_t* fd, ipv4str_t src_ip, port_t dst_port, ipv4str_t dst_ip, port_t dst_port);

status_t accept_new_connection(sockfd_t* new_fd, sockfd_t fd, ipv4str_t conn_ip, port_t* conn_port);

status_t pull_tcp_data(sockfd_t fd, Buffer buf, size_t size);

status_t push_tcp_data(sockfd_t fd, Buffer buf, size_t size);

status_t init_udp_socket(sockfd_t* fd);

status_t pull_udp_data(sockfd_t fd, ipv4str_t conn_ip, port_t* conn_port, Buffer buf, size_t size);

status_t push_udp_data(sockfd_t fd, ipv4str_t dst_ip, port_t dst_port, Buffer buf, size_t size);

status_t set_socket_rcvbufsize(sockfd_t fd, size_t size);

status_t set_socket_sndbufsize(sockfd_t fd, size_t size);

status_t set_socket_timeout(sockfd_t fd, time_t second);

status_t set_socket_broadcast(sockfd_t fd);

status_t close_socket(sockfd_t fd);

#endif
