#include "data.h"

status_t start_data(CntlAddrs *addrs, sockfd_t *sock)
{
	status_t _stat = SUCCESS;
	LOGT("in function start_data()");
	CHECK_IPV4(addrs->local_ip, "invalid local ip address");
	CHECK_IPV4(addrs->remote_ip, "invalid remote ip address");
	CHECK_PORT(addrs->local_port, "invalid local port");
	CHECK_PORT(addrs->remote_port, "invalid remote port");
	LOGD("local address is %s:%hu", addrs->local_ip, addrs->local_port);
	LOGD("remote address is %s:%hu", addrs->remote_ip, addrs->remote_port);
	CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port, false), "init_udp_socket() failed");
	LOGD("UDP socket created with fd = %d", *sock);
	LOGT("return from start_data()");
	return _stat;
}

status_t end_data(sockfd_t sock)
{
	status_t _stat = SUCCESS;
	LOGT("in function end_data()");
	CHECK_STAT(close_socket(sock), "close_socket() failed on socket with fd = %d", sock);
	LOGT("return from end_data()");
	return _stat;
}

status_t push_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout)
{
	status_t _stat = SUCCESS;
	MFILE *stream = &(filec->mfile);
	buffer_t segbuf = (buffer_t) calloc(conf->segsize, sizeof (char));
	size_t rsize = chunk->chunk_size;
	LOGT("in function push_chunk_data()");
	CHECK_PTR(segbuf, EMALLOC, "calloc() failed to allocate buffer with size = %zu", conf->segsize);
	mfseek(stream, chunk->start_pos);
	LOGD("MFILE position set to %lu", chunk->start_pos);
	LOGD("the chunk prepared and it is ready to push");
	while (rsize != 0) {
		size_t segsize = (conf->segsize <= rsize) ? conf->segsize : rsize;
		struct pollfd pfd = {.fd = sock, .events = POLLOUT};
		mfread(segbuf, segsize, sizeof (char), stream);
		if (segsize > 8)
			LOGD("read %zu bytes of the chunk", segsize);
		else
			LOGD("read %zu bytes of the chunk with value %s", segsize, segbuf);
		switch (poll(&pfd, 1, timeout)) {
		case -1 :
			free(segbuf);
			return _stat = FAILURE;
		case 0 :
			free(segbuf);
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLOUT) ? SUCCESS : ERRPOLL;
			CHECK_SSTAT(_stat, segbuf, "poll() failed on socket with fd = %d", sock);
		}
		CHECK_SSTAT(push_udp_data(sock, segbuf, segsize), segbuf, "push_udp_data() failed to push segment on socket with fd = %d", sock);
		LOGD("segment with size %zu pushed", segsize);
		memset(segbuf, 0, conf->segsize);
		rsize -= segsize;
		LOGD("%zu bytes of the chunk remain", rsize);
	}
	free(segbuf);
	LOGD("the chunk with size %zu pushed", chunk->chunk_size);
	LOGT("return from push_chunk_data()");
	return _stat;
}

status_t pull_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout)
{
	status_t _stat = SUCCESS;
	MFILE *stream = &(filec->mfile);
	buffer_t segbuf = (buffer_t) calloc(conf->segsize, sizeof (char));
	size_t rsize = chunk->chunk_size;
	LOGT("in function pull_chunk_data()");
	CHECK_PTR(segbuf, EMALLOC, "calloc() failed to allocate buffer with size = %zu", conf->segsize);
	mfseek(stream, chunk->start_pos);
	LOGD("MFILE position set to %lu", chunk->start_pos);
	CHECK_SSTAT(set_socket_rcvlowsize(sock, 2 * conf->segsize), segbuf, "set_socket_rcvlowsize() failed");
	LOGD("socket SO_RCVLOWAT set to %zu", conf->segsize);
	while (rsize != 0) {
		size_t segsize = (conf->segsize <= rsize) ? conf->segsize : rsize;
		struct pollfd pfd = {.fd = sock, .events = POLLIN};
		switch (poll(&pfd, 1, timeout)) {
		case -1 :   
			free(segbuf);
			return _stat = FAILURE;
		case 0 :
			free(segbuf);
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLIN) ? pull_udp_data(sock, segbuf, segsize) : ERRPOLL;
			CHECK_SSTAT(_stat, segbuf, "pull_udp_data() failed to pull segment on socket with fd = %d", sock);
			LOGD("segment with size %zu pulled", segsize);
		}
		mfwrite(segbuf, segsize, sizeof (char), stream);
		LOGD("write %zu bytes of the chunk", segsize);
		memset(segbuf, 0, conf->segsize);
		rsize -= segsize;
		LOGD("%zu bytes of the chunk remain", rsize);
	}
	free(segbuf);
	LOGD("the chunk with size %zu pulled", chunk->chunk_size);
	LOGT("return from pull_chunk_data()");
	return _stat;
}
