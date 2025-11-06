#include "data.h"

status_t start_data(CntlAddrs *addrs, sockfd_t *sock) {
	status_t _stat = SUCCESS;
	LOGT(__FILE__, __func__, "start data");
	LOGD(__FILE__, __func__, "local ip = %s", addrs->local_ip);
	CHECK_BOOL(check_ipv4_format(addrs->local_ip), BADIPV4);
	LOGD(__FILE__, __func__, "remote ip = %s", addrs->remote_ip);
	CHECK_BOOL(check_ipv4_format(addrs->remote_ip), BADIPV4);
	LOGD(__FILE__, __func__, "local port = %hu", addrs->local_port);
	CHECK_PORT(addrs->local_port);
	LOGD(__FILE__, __func__, "remote port = %hu", addrs->remote_port);
	CHECK_PORT(addrs->remote_port);
	CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port));
	return _stat;
}

status_t end_data(sockfd_t sock) {
	status_t _stat = SUCCESS;
	LOGT(__FILE__, __func__, "end data with socket fd = %d", sock);
	CHECK_INT(close(sock), FAILURE);
	return _stat;
}

status_t push_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout) {
	status_t _stat = SUCCESS;
	MFILE *stream = &(filec->mfile);
	unsigned char segbuf[SEGMENTSIZE];
	size_t rsize = chunk->chunk_size;
	CHECK_MFILE(filec->mfile);
	mfseek(stream, chunk->start_pos);
	while (rsize != 0) {
		size_t segsize = (SEGMENTSIZE <= rsize) ? SEGMENTSIZE : rsize;
		struct pollfd pfd = {.fd = sock, .events = POLLOUT};
		mfread(segbuf, segsize, sizeof (char), stream);
		switch (poll(&pfd, 1, timeout)) {
			case -1 :
				return _stat = FAILURE;
			case 0 :
				return _stat = TIMEOUT;
			default :
				_stat = (pfd.revents & POLLOUT) ? SUCCESS : ERRPOLL;
				CHECK_STAT(_stat);
				break;
		}
		CHECK_STAT(push_udp_data(sock, segbuf, segsize));
		memset(segbuf, 0, SEGMENTSIZE);
		rsize -= segsize;
	}
	LOGD(__FILE__, __func__, "chunk with size %lu pushed", chunk->chunk_size);
	return _stat;
}

status_t pull_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout) {
	status_t _stat = SUCCESS;
	MFILE *stream = &(filec->mfile);
	unsigned char segbuf[SEGMENTSIZE];
	size_t rsize = chunk->chunk_size;
	CHECK_MFILE(filec->mfile);
	mfseek(stream, chunk->start_pos);
	CHECK_STAT(set_socket_rcvlowsize(sock, 2 * SEGMENTSIZE));
	while (rsize != 0) {
		size_t segsize = (SEGMENTSIZE <= rsize) ? SEGMENTSIZE : rsize;
		struct pollfd pfd = {.fd = sock, .events = POLLIN};
		switch (poll(&pfd, 1, timeout)) {
			case -1 :   
				return _stat = FAILURE;
			case 0 :
				return _stat = TIMEOUT;
			default :
				_stat = (pfd.revents & POLLIN) ? SUCCESS : ERRPOLL;
				CHECK_STAT(_stat);
				break;
		}
		CHECK_STAT(pull_udp_data(sock, segbuf, segsize));
		mfwrite(segbuf, segsize, sizeof (char), stream);
		memset(segbuf, 0, SEGMENTSIZE);
		rsize -= segsize;
	}
	LOGD(__FILE__, __func__, "chunk with size %lu pulled", chunk->chunk_size);
	return _stat;
}
