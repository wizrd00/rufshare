#include "data.h"

status_t start_data(CntlAddrs *addrs, sockfd_t *sock)
{
	status_t _stat = SUCCESS;
	CHECK_IPV4(addrs->local_ip);
	CHECK_IPV4(addrs->remote_ip);
	CHECK_PORT(addrs->local_port);
	CHECK_PORT(addrs->remote_port);
	CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port, false));
	return _stat;
}

status_t end_data(sockfd_t sock)
{
	status_t _stat = SUCCESS;
	CHECK_STAT(close_socket(sock));
	return _stat;
}

status_t push_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout)
{
	status_t _stat = SUCCESS;
	MFILE *stream = &(filec->mfile);
	buffer_t segbuf = (buffer_t) calloc(conf->segsize, sizeof (char));
	size_t rsize = chunk->chunk_size;
	CHECK_PTR(segbuf, EMALLOC);
	mfseek(stream, chunk->start_pos);
	while (rsize != 0) {
		size_t segsize = (conf->segsize <= rsize) ? conf->segsize : rsize;
		struct pollfd pfd = {.fd = sock, .events = POLLOUT};
		mfread(segbuf, segsize, sizeof (char), stream);
		switch (poll(&pfd, 1, timeout)) {
			case -1 :
				return _stat = FAILURE;
			case 0 :
				return _stat = TIMEOUT;
			default :
				_stat = (pfd.revents & POLLOUT) ? SUCCESS : ERRPOLL;
				CHECK_SSTAT(_stat, segbuf);
				break;
		}
		CHECK_SSTAT(push_udp_data(sock, segbuf, segsize), segbuf);
		memset(segbuf, 0, conf->segsize);
		rsize -= segsize;
	}
	free(segbuf);
	return _stat;
}

status_t pull_chunk_data(sockfd_t sock, FileContext *filec, ChunkContext *chunk, int timeout)
{
	status_t _stat = SUCCESS;
	MFILE *stream = &(filec->mfile);
	buffer_t segbuf = (buffer_t) calloc(conf->segsize, sizeof (char));
	size_t rsize = chunk->chunk_size;
	CHECK_PTR(segbuf, EMALLOC);
	mfseek(stream, chunk->start_pos);
	CHECK_SSTAT(set_socket_rcvlowsize(sock, 2 * conf->segsize), segbuf);
	while (rsize != 0) {
		size_t segsize = (conf->segsize <= rsize) ? conf->segsize : rsize;
		struct pollfd pfd = {.fd = sock, .events = POLLIN};
		switch (poll(&pfd, 1, timeout)) {
			case -1 :   
				return _stat = FAILURE;
			case 0 :
				return _stat = TIMEOUT;
			default :
				_stat = (pfd.revents & POLLIN) ? pull_udp_data(sock, segbuf, segsize) : ERRPOLL;
				CHECK_SSTAT(_stat, segbuf);
				break;
		}
		mfwrite(segbuf, segsize, sizeof (char), stream);
		memset(segbuf, 0, conf->segsize);
		rsize -= segsize;
	}
	free(segbuf);
	return _stat;
}
