#include "data.h"

status_t start_data(CntlAddrs *addrs, sockfd_t *sock)
{
	status_t _stat = SUCCESS;
	LOGT("in function start_data()");
	CHECK_IPV4(addrs->local_ip);
	CHECK_IPV4(addrs->remote_ip);
	CHECK_PORT(addrs->local_port);
	CHECK_PORT(addrs->remote_port);
	LOGD("local address is %s:%hu", addrs->local_ip, addrs->local_port);
	LOGD("remote address is %s:%hu", addrs->remote_ip, addrs->remote_port);
	CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port, false));
	LOGD("UDP socket created with fd = %d", *sock);
	LOGT("return from start_data()");
	return _stat;
}

status_t end_data(sockfd_t sock)
{
	status_t _stat = SUCCESS;
	LOGT("in function end_data()");
	CHECK_STAT(close_socket(sock));
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
	CHECK_PTR(segbuf, EMALLOC);
	mfseek(stream, chunk->start_pos);
	LOGD("MFILE position set to %lu", chunk->start_pos);
	LOGD("the chunk prepared and it is ready to push");
	while (rsize != 0) {
		size_t segsize = (conf->segsize <= rsize) ? conf->segsize : rsize;
		struct pollfd pfd = {.fd = sock, .events = POLLOUT};
		mfread(segbuf, segsize, sizeof (char), stream);
		LOGD("read %zd bytes of the chunk", segsize);
		switch (poll(&pfd, 1, timeout)) {
			case -1 :
				free(segbuf);
				return _stat = FAILURE;
			case 0 :
				free(segbuf);
				return _stat = TIMEOUT;
			default :
				_stat = (pfd.revents & POLLOUT) ? SUCCESS : ERRPOLL;
				CHECK_SSTAT(_stat, segbuf);
		}
		CHECK_SSTAT(push_udp_data(sock, segbuf, segsize), segbuf);
		LOGD("segment with size %zd pushed", segsize);
		memset(segbuf, 0, conf->segsize);
		rsize -= segsize;
		LOGD("%zd bytes of the chunk remain", rsize);
	}
	free(segbuf);
	LOGD("the chunk with size %zd pushed", chunk->chunk_size);
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
	CHECK_PTR(segbuf, EMALLOC);
	mfseek(stream, chunk->start_pos);
	LOGD("MFILE position set to %lu", chunk->start_pos);
	CHECK_SSTAT(set_socket_rcvlowsize(sock, 2 * conf->segsize), segbuf);
	LOGD("socket SO_RCVLOWAT set to %zd", conf->segsize);
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
				CHECK_SSTAT(_stat, segbuf);
				LOGD("segment with size %zd pulled", segsize);
		}
		mfwrite(segbuf, segsize, sizeof (char), stream);
		LOGD("write %zd bytes of the chunk");
		memset(segbuf, 0, conf->segsize);
		rsize -= segsize;
		LOGD("%zd bytes of the chunk remain", rsize);
	}
	free(segbuf);
	LOGD("the chunk with size %zd pulled", chunk->chunk_size);
	LOGT("return from pull_chunk_data()");
	return _stat;
}
