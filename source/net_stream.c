#include "net_stream.h"

static status_t network_byteorder(ipv4str_t ip, uint32_t *dst)
{
	status_t _stat = SUCCESS;
	LOGT("in function network_byteorder()");
	switch (inet_pton(AF_INET, ip, dst)) {
		case -1 :
			CHECK_LSTAT(BADARGS, "invalid address family");
		case 0 :
			CHECK_LSTAT(BADIPV4, "invalid ip address");
	}
	LOGT("return from network_byteorder()");
	return _stat;
}

static status_t host_ipstring(ipv4str_t ip, struct in_addr *addr)
{
	status_t _stat = SUCCESS;
	LOGT("in function host_ipstring()");
	CHECK_LPTR(inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN), BADARGS, "inet_ntop() failed");
	LOGT("return from host_ipstring()");
	return _stat;
}

status_t init_tcp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port, bool conn)
{
	status_t _stat = SUCCESS;
	uint32_t src_ipnetorder;
	uint32_t dst_ipnetorder;
	int tmpsock;
	int optval = 1;
	LOGT("in function init_tcp_socket()");
	CHECK_LSTAT(network_byteorder(src_ip, &src_ipnetorder), "network_byteorder() failed");
	CHECK_LSTAT(network_byteorder(dst_ip, &dst_ipnetorder), "network_byteorder() failed");
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
	CHECK_LINT(tmpsock, INVSOCK, "socket() failed");
	*sock = tmpsock;
	CHECK_LINT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET, "setsockopt() failed to set SO_REUSEADDR option on socket with fd = %d", *sock);
	CHECK_LINT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND, "bind() failed on socket with fd = %d", *sock);
	if (conn)
		CHECK_LINT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN, "connect() failed on socket with fd = %d", *sock);
	LOGT("return from init_tcp_socket()");
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
	LOGT("in function accept_new_connection()");
	CHECK_LINT(listen(sock, BACKLOG), ERRLSTN, "listen() failed on socket with fd = %d", sock);
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			CHECK_LSTAT(FAILURE, "poll() failed on socket with fd = %d", sock);
		case 0 :
			CHECK_LSTAT(TIMEOUT, "poll() timeout on socket with fd = %d", sock);
	}
	tmpsock = accept(sock, (struct sockaddr *) &conn_addr, &addr_len);
	CHECK_LINT(tmpsock, INVSOCK, "accept() failed on socket with fd = %d", sock);
	*new_sock = tmpsock;
	CHECK_LEQUAL(AF_INET, conn_addr.ss_family, BADINET, "conn_addr.ss != AF_INET");
	tmp_addr = (struct sockaddr_in *) &conn_addr;
	CHECK_LSTAT(host_ipstring(conn_ip, &(tmp_addr->sin_addr)), "host_ipstring() failed");
	*conn_port = ntohs(tmp_addr->sin_port);
	LOGT("return from accept_new_connection()");
	return _stat;
}

status_t push_tcp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	LOGT("in function push_tcp_data()");
	ssize_t send_size = send(sock, buf, size, MSG_NOSIGNAL);
	CHECK_LINT(send_size, ERRSEND, "send() failed on socket with fd = %d", sock);
	CHECK_LSIZE((size_t) send_size, size, "send() pushed less than expected size %zd bytes", size);
	LOGT("return from push_tcp_data");
	return _stat;
}

status_t pull_tcp_data(sockfd_t sock, buffer_t buf, size_t size, bool peek_flag)
{
	status_t _stat = SUCCESS;
	LOGT("in function pull_tcp_data()");
	ssize_t recv_size = recv(sock, buf, size, (peek_flag) ? MSG_PEEK : 0);
	CHECK_LINT(recv_size, ERRRECV, "recv() failed on socket with fd = %d", sock);
	CHECK_LSIZE((size_t) recv_size, size, "recv() pulled less than expected size %zd bytes", size);
	LOGT("return from pull_tcp_data()");
	return _stat;
}

status_t init_udp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port, bool broadcast)
{
	status_t _stat = SUCCESS;
	uint32_t src_ipnetorder;
	uint32_t dst_ipnetorder;
	int tmpsock;
	int optval = 1;
	LOGT("in function init_udp_socket()");
	CHECK_LSTAT(network_byteorder(src_ip, &src_ipnetorder), "network_byteorder() failed");
	CHECK_LSTAT(network_byteorder(dst_ip, &dst_ipnetorder), "network_byteorder() failed");
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
	CHECK_LINT(tmpsock, INVSOCK, "socket() failed");
	*sock = tmpsock;
	if (broadcast)
		CHECK_LINT(setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (int)), FAILSET, "setsockopt() failed to set SO_BROADCAST on socket with fd = %d", *sock);
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET, "setsockopt() failed to set SO_REUSEADDR on socket with fd = %d", *sock);
	CHECK_LINT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND, "bind() failed on socket with fd = %d", *sock);
	CHECK_LINT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN, "connect() failed on socket with fd = %d", *sock);
	LOGT("return from init_udp_socket()");
	return _stat;
}

status_t push_udp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	LOGT("in function push_udp_data()");
	ssize_t send_size = send(sock, buf, size, 0);
	CHECK_LINT(send_size, ERRSEND, "send() failed on socket with fd = %d", sock);
	CHECK_LSIZE((size_t) send_size, size, "send() pushed less than expected size %zd bytes", size);
	LOGT("return from push_udp_data()");
	return _stat;
}

status_t pull_udp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	LOGT("in function pull_udp_data()");
	ssize_t recv_size = recv(sock, buf, size, 0);
	CHECK_LINT(recv_size, ERRRECV, "recv() failed on socket with fd = %d", sock);
	CHECK_LSIZE((size_t) recv_size, size, "recv() pulled less than expected size %zd bytes", size);
	LOGT("return from pull_udp_data()");
	return _stat;
}

status_t set_socket_rcvlowsize(sockfd_t sock, size_t size)
{
	status_t _stat = SUCCESS;
	LOGT("in function set_socket_rcvlowsize()");
	CHECK_LINT(setsockopt(sock, SOL_SOCKET, SO_RCVLOWAT , &size, sizeof (size_t)), FAILSET, "setsockopt() failed to set SO_RCVLOWAT on socket with fd = %d", sock);
	LOGT("return from set_socket_rcvlowsize()");
	return _stat;
}

status_t set_socket_sndlowsize(sockfd_t sock, size_t size) {
	status_t _stat = SUCCESS;
	LOGT("in function set_socket_sndlowsize()");
	CHECK_LINT(setsockopt(sock, SOL_SOCKET, SO_SNDLOWAT , &size, sizeof (size_t)), FAILSET, "setsockopt() failed to set SO_SNDLOWAT on sockket with fd = %d", sock);
	LOGT("return from set_socket_sndlowsize()");
	return _stat;
}

status_t set_socket_rcvbufsize(sockfd_t sock, size_t size)
{
	status_t _stat = SUCCESS;
	LOGT("in function set_socket_rcvbufsize()");
	CHECK_LINT(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof (size_t)), FAILSET, "setsockopt() failed to set SO_RCVBUF on socket with fd = %d", sock);
	LOGT("return from set_socket_rcvbufsize()");
	return _stat;
}

status_t set_socket_sndbufsize(sockfd_t sock, size_t size)
{
	status_t _stat = SUCCESS;
	LOGT("in function set_socket_sndbufsize()");
	CHECK_LINT(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof (size_t)), FAILSET, "setsockopt() failed to set SO_SNDBUF on socket with fd = %d", sock);
	LOGT("return from set_socket_sndbufsize()");
	return _stat;
}

status_t set_socket_timeout(sockfd_t sock, time_t second)
{
	status_t _stat = SUCCESS;
	struct timeval timeout = {
		.tv_sec = second,
		.tv_usec = 0
	};
	LOGT("in function set_socket_timeout()");
	CHECK_LINT(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (struct timeval)), FAILSET, "setsockopt() failed to set SO_RCVTIMEO on socket with fd = %d", sock);
	CHECK_LINT(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (struct timeval)), FAILSET, "setsockopt() failed to set SO_SNDTIMEO on socket with fd = %d", sock);
	LOGT("return from set_socket_timeout()");
	return _stat;
}

status_t close_socket(sockfd_t sock)
{
	status_t _stat = SUCCESS;
	LOGT("in function close_socket()");
	CHECK_LINT(close(sock), FAILURE, "close() failed to close socket with fd = %d", sock);
	LOGT("return from close_socket()");
	return _stat;
}
