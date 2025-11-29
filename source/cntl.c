#include "cntl.h"

status_t start_cntl(CntlAddrs *addrs, sockfd_t *sock, bool conn) {
	status_t _stat = SUCCESS;
	LOGT(__FILE__, __func__, "start cntl");
	LOGD(__FILE__, __func__, "local ip = %s", addrs->local_ip);
	CHECK_BOOL(check_ipv4_format(addrs->local_ip), BADIPV4);
	LOGD(__FILE__, __func__, "remote ip = %s", addrs->remote_ip);
	CHECK_BOOL(check_ipv4_format(addrs->remote_ip), BADIPV4);
	LOGD(__FILE__, __func__, "local port = %hu", addrs->local_port);
	CHECK_PORT(addrs->local_port);
	LOGD(__FILE__, __func__, "remote port = %hu", addrs->remote_port);
	CHECK_PORT(addrs->remote_port);
	CHECK_STAT(init_tcp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port, conn));
	return _stat;
}

status_t accept_cntl(CntlAddrs *addrs, sockfd_t *new_sock, sockfd_t sock, int timeout) {
	status_t _stat = SUCCESS;
	LOGT(__FILE__, __func__, "start accept cntl headers");
	CHECK_STAT(accept_new_connection(new_sock, sock, addrs->remote_ip, &(addrs->remote_port), timeout));
	return _stat;
}

status_t end_cntl(sockfd_t sock) {
	status_t _stat = SUCCESS;
	LOGT(__FILE__, __func__, "end cntl with socket fd = %d", sock);
	CHECK_INT(close(sock), FAILURE);
	return _stat;
}

status_t push_CAST_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	CHECK_PTR(buf, EMALLOC);
	pack_into_infostring(infostr, &(args->cast.info));
	memcpy((void *) buf, (void *) &(args->cast.packet), sizeof (CastPacket));
	memcpy((void *) buf + sizeof (CastPacket), (void *) infostr, sizeof (infostr));
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			_stat = FAILURE;
			break;
		case 0 :
			_stat = TIMEOUT;
			break;
		default :
			_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf);
			break;
	}
	LOGD(__FILE__, __func__, "CAST header pushed");
	free(buf);
	return _stat;
}

status_t push_FLOW_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	size_t bufsize = sizeof (FlowPacket);
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	CHECK_PTR(buf, EMALLOC);
	memcpy((void *) buf, (void *) &(args->flow.packet), sizeof (FlowPacket));
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			_stat = FAILURE;
			break;
		case 0 :
			_stat = TIMEOUT;
			break;
		default :
			_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf);
			break;
	}
	LOGD(__FILE__, __func__, "FLOW header with chunk_size = %hu, sequence = %lu pushed", args->flow.packet.chunk_size, args->flow.packet.sequence);
	free(buf);
	return _stat;
}

status_t push_SEND_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (SendPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	CHECK_PTR(buf, EMALLOC);
	pack_into_infostring(infostr, &(args->send.info));
	memcpy((void *) buf, (void *) &(args->send.packet), sizeof (SendPacket));
	memcpy((void *) buf + sizeof (SendPacket), (void *) infostr, sizeof (infostr));
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			_stat = FAILURE;
			break;
		case 0 :
			_stat = TIMEOUT;
			break;
		default :
			_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf);
			break;
	}
	LOGT(__FILE__, __func__, "SEND header with chunk_size = %hu, chunk_count = %lu, partial_chunk_size = %hu pushed", args->send.packet.chunk_size,args->send.packet.chunk_count,args->send.packet.partial_chunk_size);
	free(buf);
	return _stat;
}

status_t push_RECV_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	size_t bufsize = sizeof (RecvPacket);
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	CHECK_PTR(buf, EMALLOC);
	memcpy((void *) buf, (void *) &(args->recv.packet), sizeof (RecvPacket));
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			_stat = FAILURE;
			break;
		case 0 :
			_stat = TIMEOUT;
			break;
		default :
			_stat = (pfd.revents & POLLOUT) ? push_tcp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf);
			break;
	}
	LOGD(__FILE__, __func__, " RECV header with ack = %u pushed", args->recv.packet.ack);
	free(buf);
	return _stat;
}

status_t pull_CAST_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	CastPacket packet;
	char infostr[INFOSTRSIZE] = {0};
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			return _stat = FAILURE;
		case 0 :
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
			CHECK_STAT(_stat);
			CHECK_EQUAL(CAST, tmp_type, BADTYPE);
			break;
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (CastPacket), false));
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) infostr, INFOSTRSIZE, false));
	args->cast.packet = convert_CastPacket_byteorder(&packet);
	unpack_from_infostring(infostr, &(args->cast.info));
	LOGD(__FILE__, __func__, "CAST header with %s:%s:%hu pulled", args->cast.info.name, args->cast.info.remote_ip, args->cast.info.remote_port);
	return _stat;
}

status_t pull_FLOW_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	FlowPacket packet;
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			return _stat = FAILURE;
		case 0 :
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
			CHECK_STAT(_stat);
			CHECK_EQUAL(FLOW, tmp_type, BADTYPE);
			break;
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (FlowPacket), false));
	args->flow.packet = convert_FlowPacket_byteorder(&packet);
	LOGD(__FILE__, __func__, "FLOW header with chunk_size = %hu, sequence = %lu pulled", args->flow.packet.chunk_size, args->flow.packet.sequence);
	return _stat;
}

status_t pull_SEND_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	SendPacket packet;
	char infostr[INFOSTRSIZE] = {0};
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			return _stat = FAILURE;
		case 0 :
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
			CHECK_STAT(_stat);
			CHECK_EQUAL(SEND, tmp_type, BADTYPE);
			break;
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (SendPacket), false));
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) infostr, INFOSTRSIZE, false));
	args->send.packet = convert_SendPacket_byteorder(&packet);
	unpack_from_infostring(infostr, &(args->send.info));
	LOGD(__FILE__, __func__, "SEND header with chunk_size = %hu, chunk_count = %lu, partial_chunk_size = %hu pulled", args->send.packet.chunk_size, args->send.packet.chunk_count, args->send.packet.partial_chunk_size);
	return _stat;
}

status_t pull_RECV_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	RecvPacket packet;
	RUFShareType tmp_type;
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			return _stat = FAILURE;
		case 0 :
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLIN) ? pull_tcp_data(sock, (buffer_t) &tmp_type, sizeof (RUFShareType), true) : ERRPOLL;
			CHECK_STAT(_stat);
			CHECK_EQUAL(RECV, tmp_type, BADTYPE);
			break;
	}
	CHECK_STAT(pull_tcp_data(sock, (buffer_t) &packet, sizeof (RecvPacket), false));
	args->recv.packet = convert_RecvPacket_byteorder(&packet);
	LOGD(__FILE__, __func__, "RECV header with ack = %u pulled", args->recv.packet.ack);
	return _stat;
}
