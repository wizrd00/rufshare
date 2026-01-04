#include "cntl.h"

status_t start_cntl(CntlAddrs *addrs, sockfd_t *sock, bool conn)
{
	status_t _stat = SUCCESS;
	LOGT("in function start_cntl()");
	CHECK_IPV4(addrs->local_ip, "invalid local ip address");
	CHECK_IPV4(addrs->remote_ip, "invalid remote ip address");
	CHECK_PORT(addrs->local_port, "invalid local port");
	CHECK_PORT(addrs->remote_port, "invalid remote port");
	LOGD("local address is %s:%hu", addrs->local_ip, addrs->local_port);
	LOGD("remote address is %s:%hu", addrs->remote_ip, addrs->remote_port);
	CHECK_STAT(init_tcp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port, conn), "init_tcp_socket() failed");
	LOGD("TCP socket created with fd = %d", *sock);
	LOGT("return from start_cntl()");
	return _stat;
}

status_t accept_cntl(CntlAddrs *addrs, sockfd_t *new_sock, sockfd_t sock, int timeout)
{
	status_t _stat = SUCCESS;
	LOGT("in function accept_cntl()");
	LOGD("accept new connect on socket with fd = %d", sock);
	CHECK_STAT(accept_new_connection(new_sock, sock, addrs->remote_ip, &(addrs->remote_port), timeout), "accept_new_connection() failed on socket with fd = %d", sock);
	LOGD("new connection on TCP socket with fd = %d", *new_sock);
	LOGT("return from accept_ctnl()");
	return _stat;
}

status_t end_cntl(sockfd_t sock)
{
	status_t _stat = SUCCESS;
	LOGT("in function end_cntl()");
	CHECK_STAT(close_socket(sock), "close_socket() failed on socket with fd = %d", sock);
	LOGT("return from end_cntl()");
	return _stat;
}

status_t push_CAST_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	LOGT("in function push_CAST_header()");
	CHECK_PTR(buf, EMALLOC, "malloc() failed to allocate buffer with size = %zu", bufsize);
	pack_into_infostring(infostr, &(args->cast.info));
	memcpy((void *) buf, (void *) &(args->cast.packet), sizeof (CastPacket));
	memcpy((void *) buf + sizeof (CastPacket), (void *) infostr, sizeof (infostr));
	LOGD("CAST packet prepared and it is ready to push");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		free(buf);
		_stat = FAILURE;
		break;
	case 0 :
		free(buf);
		_stat = TIMEOUT;
		break;
	default :
		_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
		CHECK_SSTAT(_stat, buf, "push_tcp_data() failed on socket with fd = %d", sock);
		LOGD("CAST packet pushed");
	}
	free(buf);
	LOGT("return from push_CAST_header()");
	return _stat;
}

status_t push_FLOW_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	size_t bufsize = sizeof (FlowPacket);
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	LOGT("in function push_FLOW_header()");
	CHECK_PTR(buf, EMALLOC, "malloc() failed to allocate buffer with size = %zu", bufsize);
	memcpy((void *) buf, (void *) &(args->flow.packet), sizeof (FlowPacket));
	LOGD("FLOW packet prepared and it is ready to push");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		free(buf);
		_stat = FAILURE;
		break;
	case 0 :
		free(buf);
		_stat = TIMEOUT;
		break;
	default :
		_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
		CHECK_SSTAT(_stat, buf, "push_tcp_data() failed on socket with fd = %d", sock);
		LOGD("FLOW packet pushed");
	}
	free(buf);
	LOGT("return from push_FLOW_header()");
	return _stat;
}

status_t push_SEND_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (SendPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	LOGT("in function push_SEND_header()");
	CHECK_PTR(buf, EMALLOC, "malloc() failed to allocate buffer with size = %zu", bufsize);
	pack_into_infostring(infostr, &(args->send.info));
	memcpy((void *) buf, (void *) &(args->send.packet), sizeof (SendPacket));
	memcpy((void *) buf + sizeof (SendPacket), (void *) infostr, sizeof (infostr));
	LOGD("SEND packet prepared and it is ready to push");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		free(buf);
		_stat = FAILURE;
		break;
	case 0 :
		free(buf);
		_stat = TIMEOUT;
		break;
	default :
		_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
		CHECK_SSTAT(_stat, buf, "push_tcp_data() failed on socket with fd = %d", sock);
		LOGD("SEND packet pushed");
	}
	free(buf);
	LOGT("return from push_SEND_header()");
	return _stat;
}

status_t push_RECV_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	size_t bufsize = sizeof (RecvPacket);
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	LOGT("in function push_RECV_header()");
	CHECK_PTR(buf, EMALLOC, "malloc() failed to allocate buffer with size = %zu", bufsize);
	memcpy((void *) buf, (void *) &(args->recv.packet), sizeof (RecvPacket));
	LOGD("RECV packet prepared and it is ready to push");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		free(buf);
		_stat = FAILURE;
		break;
	case 0 :
		free(buf);
		_stat = TIMEOUT;
		break;
	default :
		_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
		CHECK_SSTAT(_stat, buf, "push_tcp_data() failed on socket with fd = %d", sock);
		LOGD("RECV packet pushed");
	}
	free(buf);
	LOGT("return from push_RECV_header()");
	return _stat;
}

status_t pull_CAST_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	CastPacket packet;
	char infostr[INFOSTRSIZE] = {0};
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	LOGT("in function pull_CAST_header()");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		return _stat = FAILURE;
	case 0 :
		return _stat = TIMEOUT;
	default :
		_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
		CHECK_STAT(_stat, "pull_tcp_data() failed to pull type on socket with fd = %d", sock);
		CHECK_EQUAL(CAST, tmp_type, BADTYPE, "invalid type != CAST");
		LOGD("CAST packet pulled");
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (CastPacket), false), "pull_tcp_data() failed to pull CAST packet on socket with fd = %d", sock);
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) infostr, INFOSTRSIZE, false), "pull_tcp_data() failed to pull infostr on socket with fd = %d", sock);
	args->cast.packet = convert_CastPacket_byteorder(&packet);
	unpack_from_infostring(infostr, &(args->cast.info));
	LOGD("CAST packet unpacked");
	LOGT("return from pull_CAST_header()");
	return _stat;
}

status_t pull_FLOW_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	FlowPacket packet;
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	LOGT("in function pull_FLOW_header()");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		return _stat = FAILURE;
	case 0 :
		return _stat = TIMEOUT;
	default :
		_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
		CHECK_STAT(_stat, "pull_tcp_data() failed to pull type on socket with fd = %d", sock);
		CHECK_EQUAL(FLOW, tmp_type, BADTYPE, "invalid type != FLOW");
		LOGD("FLOW packet pulled");
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (FlowPacket), false), "pull_tcp_data() failed to pull FLOW packet on socket with fd = %d", sock);
	args->flow.packet = convert_FlowPacket_byteorder(&packet);
	LOGD("FLOW packet unpacked");
	LOGT("return from pull_FLOW_header()");
	return _stat;
}

status_t pull_SEND_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	SendPacket packet;
	char infostr[INFOSTRSIZE] = {0};
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	LOGT("in function pull_SEND_header()");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		return _stat = FAILURE;
	case 0 :
		return _stat = TIMEOUT;
	default :
		_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
		CHECK_STAT(_stat, "pull_tcp_data() failed to pull type on socket with fd = %d", sock);
		CHECK_EQUAL(SEND, tmp_type, BADTYPE, "invalid type != SEND");
		LOGD("SEND packet pulled");
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (SendPacket), false), "pull_tcp_data() failed to pull SEND packet on socket with fd = %d", sock);
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) infostr, INFOSTRSIZE, false), "pull_tcp_data() failed to pull infostr on socket with fd = %d", sock);
	LOGD("infostr = %s", infostr);
	args->send.packet = convert_SendPacket_byteorder(&packet);
	unpack_from_infostring(infostr, &(args->send.info));
	LOGD("SEND packet unpacked");
	LOGT("return from pull_SEND_header()");
	return _stat;
}

status_t pull_RECV_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	RecvPacket packet;
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	LOGT("in function pull_RECV_header()");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		return _stat = FAILURE;
	case 0 :
		return _stat = TIMEOUT;
	default :
		_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
		CHECK_STAT(_stat, "pull_tcp_data() failed to pull type on socket with fd = %d", sock);
		CHECK_EQUAL(RECV, tmp_type, BADTYPE, "invalid type != RECV");
		LOGD("RECV packet pulled");
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (RecvPacket), false), "pull_tcp_data() failed to pull RECV packet on socket with fd = %d", sock);
	args->recv.packet = convert_RecvPacket_byteorder(&packet);
	LOGD("RECV packet unpacked");
	LOGT("return from pull_RECV_header()");
	return _stat;
}
