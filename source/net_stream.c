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

status_t init_tcp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port, ipv4str_t dst_ip, port_t dst_port, bool conn) {
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
	LOGT(__FILE__, __func__, "init tcp socket with SO_REUSEADDR option");
	tmpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	CHECK_INT(tmpsock, INVSOCK);
	*sock = tmpsock;
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET);
	LOGT(__FILE__, __func__, "bind to address %s:%hu", src_ip, src_port);
	CHECK_INT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND);
	if (conn)
		LOGT(__FILE__, __func__, "connect to address %s:%hu", dst_ip, dst_port);
		CHECK_INT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN);
	LOGT(__FILE__, __func__, "tcp socket created successfully with fd = %d", *sock);
	return stat;
}

status_t accept_new_connection(sockfd_t *new_sock, sockfd_t sock, ipv4str_t conn_ip, port_t *conn_port) {
	status_t stat = SUCCESS;
	struct sockaddr_storage conn_addr;
	struct sockaddr_in *tmp_addr;
	socklen_t addr_len;
	int tmpsock;
	LOGT(__FILE__, __func__, "listen on socket with BACKLOG = %d", BACKLOG);
	CHECK_INT(listen(sock, BACKLOG), FAILURE);
	LOGT(__FILE__, __func__, "accept on socket");
	tmpsock = accept(sock, (struct sockaddr *) &conn_addr, &addr_len);
	CHECK_INT(tmpsock, INVSOCK);
	*new_sock = tmpsock;
	CHECK_EQUAL(AF_INET, conn_addr.ss_family, BADINET);
	tmp_addr = (struct sockaddr_in *) &conn_addr;
	CHECK_STAT(host_ipstring(conn_ip, &(tmp_addr->sin_addr)));
	*conn_port = ntohs(tmp_addr->sin_port);
	LOGT(__FILE__, __func__, "new connection with address %s:%hu accepted successfully", conn_ip, *conn_port);
	return stat;
}

status_t pull_tcp_data(sockfd_t sock, Buffer buf, size_t size, bool peek_flag) {
	status_t stat = SUCCESS;
	ssize_t recv_size = recv(sock, buf, size, MSG_WAITALL | ((peek_flag) ? MSG_PEEK : 0));
	CHECK_INT(recv_size, ERRRECV);
	CHECK_SIZE((size_t) recv_size, size);
	return stat;
}

status_t push_tcp_data(sockfd_t sock, Buffer buf, size_t size) {
	status_t stat = SUCCESS;
	ssize_t send_size = send(sock, buf, size, MSG_NOSIGNAL);
	CHECK_INT(send_size, ERRSEND);
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
	LOGT(__FILE__, __func__, "init udp socket with SO_REUSEADDR option");
	tmpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	CHECK_INT(tmpsock, INVSOCK);
	*sock = tmpsock;
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET);
	LOGT(__FILE__, __func__, "bind to address %s:%hu", src_ip, src_port);
	CHECK_INT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND);
	LOGT(__FILE__, __func__, "connect to address %s:%hu", dst_ip, dst_port);
	CHECK_INT(connect(*sock, (struct sockaddr *) &remote_addr, sizeof (struct sockaddr_in)), ERRCONN);
	LOGT(__FILE__, __func__, "udp socket created successfully with fd = %d", *sock);
	return stat;
}

status_t pull_udp_data(sockfd_t sock, Buffer buf, size_t size) {
	status_t stat = SUCCESS;
	ssize_t recv_size = recv(sock, buf, size, 0);
	CHECK_INT(recv_size, ERRRECV);
	CHECK_SIZE((size_t) recv_size, size);
	return stat;
}

status_t push_udp_data(sockfd_t sock, Buffer buf, size_t size) {
	status_t stat = SUCCESS;
	ssize_t send_size = send(sock, buf, size, 0);
	CHECK_INT(send_size, ERRSEND);
	CHECK_SIZE((size_t) send_size, size);
	return stat;
}

status_t set_socket_rcvlowsize(sockfd_t sock, size_t size) {
	status_t stat = SUCCESS;
	LOGT(__FILE__, __func__, "set socket option SO_RCVLOWAT to size %lu", size);
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVLOWAT , &size, sizeof (size_t)), FAILSET);
	return stat;
}

status_t set_socket_sndlowsize(sockfd_t sock, size_t size) {
	status_t stat = SUCCESS;
	LOGT(__FILE__, __func__, "set socket option SO_SNDLOWAT to size %lu", size);
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDLOWAT , &size, sizeof (size_t)), FAILSET);
	return stat;
}

status_t set_socket_rcvbufsize(sockfd_t sock, size_t size) {
	status_t stat = SUCCESS;
	LOGT(__FILE__, __func__, "set socket option SO_RCVBUF to size %lu", size);
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof (size_t)), FAILSET);
	return stat;
}

status_t set_socket_sndbufsize(sockfd_t sock, size_t size) {
	status_t stat = SUCCESS;
	LOGT(__FILE__, __func__, "set socket option SO_SNDBUF to size %lu", size);
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof (size_t)), FAILSET);
	return stat;
}

status_t set_socket_timeout(sockfd_t sock, time_t second) {
	status_t stat = SUCCESS;
	struct timeval timeout = {
		.tv_sec = second,
		.tv_usec = 0
	};
	LOGT(__FILE__, __func__, "set socket timeout to %.6f", second);
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (struct timeval)), FAILSET);
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (struct timeval)), FAILSET);
	return stat;
}

status_t set_socket_broadcast(sockfd_t sock) {
	status_t stat = SUCCESS;
	int optval = 1;
	LOGT(__FILE__, __func__, "set socket option SO_BROADCAST on");
	CHECK_INT(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (int)), FAILSET);
	return stat;
}

status_t close_socket(sockfd_t sock) {
	status_t stat = SUCCESS;
	LOGT(__FILE__, __func__, "closing socket with fd = %d", sock);
	CHECK_INT(close(sock), FAILURE);
	return stat;
}
