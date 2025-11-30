#include "net_stream.h"

static status_t network_byteorder(ipv4str_t ip, uint32_t *dst)
{
	status_t _stat = SUCCESS;
	switch (inet_pton(AF_INET, ip, dst)) {
		case -1 :
			_stat = BADARGS;
			break;
		case 0 :
			_stat = BADIPV4;
			break;
	}
	return _stat;
}

static status_t host_ipstring(ipv4str_t ip, struct in_addr *addr)
{
	status_t _stat = SUCCESS;
	CHECK_PTR(inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN), BADARGS);
	return _stat;
}

status_t init_tcp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port, bool conn)
{
	status_t _stat = SUCCESS;
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
	if (conn)
		CHECK_INT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN);
	return _stat;
}

status_t accept_new_connection(sockfd_t *new_sock, sockfd_t sock, ipv4str_t conn_ip, port_t *conn_port, int timeout)
{
	status_t _stat = SUCCESS;
	struct sockaddr_storage conn_addr;
	struct sockaddr_in *tmp_addr;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	socklen_t addr_len;
	int tmpsock;
	CHECK_INT(listen(sock, BACKLOG), ERRLSTN);
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			return _stat = FAILURE;
		case 0 :
			return _stat = TIMEOUT;
	}
	tmpsock = accept(sock, (struct sockaddr *) &conn_addr, &addr_len);
	CHECK_INT(tmpsock, INVSOCK);
	*new_sock = tmpsock;
	CHECK_EQUAL(AF_INET, conn_addr.ss_family, BADINET);
	tmp_addr = (struct sockaddr_in *) &conn_addr;
	CHECK_STAT(host_ipstring(conn_ip, &(tmp_addr->sin_addr)));
	*conn_port = ntohs(tmp_addr->sin_port);
	return _stat;
}

status_t pull_tcp_data(sockfd_t sock, buffer_t buf, size_t size, bool peek_flag)
{
	status_t _stat = SUCCESS;
	ssize_t recv_size = recv(sock, buf, size, (peek_flag) ? MSG_PEEK : 0);
	CHECK_INT(recv_size, ERRRECV);
	CHECK_SIZE((size_t) recv_size, size);
	return _stat;
}

status_t push_tcp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	ssize_t send_size = send(sock, buf, size, MSG_NOSIGNAL);
	CHECK_INT(send_size, ERRSEND);
	CHECK_SIZE((size_t) send_size, size);
	return _stat;
}

status_t init_udp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port, bool broadcast)
{
	status_t _stat = SUCCESS;
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
	if (broadcast)
		CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (int)), FAILSET);
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET);
	CHECK_INT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND);
	CHECK_INT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN);
	return _stat;
}

status_t pull_udp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	ssize_t recv_size = recv(sock, buf, size, 0);
	CHECK_INT(recv_size, ERRRECV);
	CHECK_SIZE((size_t) recv_size, size);
	return _stat;
}

status_t push_udp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	ssize_t send_size = send(sock, buf, size, 0);
	CHECK_INT(send_size, ERRSEND);
	CHECK_SIZE((size_t) send_size, size);
	return _stat;
}

status_t set_socket_rcvlowsize(sockfd_t sock, size_t size)
{
	status_t _stat = SUCCESS;
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVLOWAT , &size, sizeof (size_t)), FAILSET);
	return _stat;
}

status_t set_socket_sndlowsize(sockfd_t sock, size_t size) {
	status_t _stat = SUCCESS;
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDLOWAT , &size, sizeof (size_t)), FAILSET);
	return _stat;
}

status_t set_socket_rcvbufsize(sockfd_t sock, size_t size)
{
	status_t _stat = SUCCESS;
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof (size_t)), FAILSET);
	return _stat;
}

status_t set_socket_sndbufsize(sockfd_t sock, size_t size)
{
	status_t _stat = SUCCESS;
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof (size_t)), FAILSET);
	return _stat;
}

status_t set_socket_timeout(sockfd_t sock, time_t second)
{
	status_t _stat = SUCCESS;
	struct timeval timeout = {
		.tv_sec = second,
		.tv_usec = 0
	};
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (struct timeval)), FAILSET);
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (struct timeval)), FAILSET);
	return _stat;
}

status_t close_socket(sockfd_t sock)
{
	status_t _stat = SUCCESS;
	CHECK_INT(close(sock), FAILURE);
	return _stat;
}
