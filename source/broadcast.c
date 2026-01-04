#include "broadcast.h"

static status_t push_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	LOGT("in function push_broadcast_header()");
	CHECK_PTR(buf, EMALLOC, "malloc() failed to allocate buffer with size = %zu", bufsize);
	LOGD("pack args->cast.info into infostr");
	pack_into_infostring(infostr, &(args->cast.info));
	memcpy((void *) buf, (void *) &(args->cast.packet), sizeof (CastPacket));
	memcpy((void *) buf + sizeof (CastPacket), (void *) infostr, sizeof (infostr));
	LOGD("CAST packet prepared and it is ready to push");
	switch (poll(&pfd, 1, timeout)) {
	case -1 :
		LOGE("poll() failed on socket with fd = %d", sock);
		free(buf);
		_stat = FAILURE;
		break;
	case 0 :
		LOGE("poll() timeout on socket with fd = %d", sock);
		free(buf);
		_stat = TIMEOUT;
		break;
	default :
		_stat = (pfd.revents & POLLOUT) ? push_udp_data(sock, buf, bufsize) : ERRPOLL;
		CHECK_SSTAT(_stat, buf, "push_udp_data() failed on socket with fd = %d", sock);
		LOGD("CAST packet pushed");
	}
        free(buf);
	LOGT("return from push_broadcast_header()");
        return _stat;
}

status_t start_broadcast(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	int trycount = conf->bc_trycount;
	LOGT("in function start_broadcast()");
	LOGD("trycount = %hd", conf->bc_trycount);
	LOGD("local address is %s:%hu", conf->addrs.local_ip, conf->addrs.local_port);
	LOGD("remote address is %s:%hu", conf->addrs.remote_ip, conf->addrs.remote_port);
	CHECK_STAT(init_udp_socket(&conf->cast_sock, conf->addrs.local_ip, conf->addrs.local_port, conf->addrs.remote_ip, conf->addrs.remote_port, true), "init_udp_socket() failed");
	LOGD("UDP broadcast socket created with fd = %d", conf->cast_sock);
	header.cast.packet = pack_RUFShare_CastPacket(0);
	sstrncpy(header.cast.info.filename, conf->addrs.filename, MAXFILENAMESIZE);
	sstrncpy(header.cast.info.name, conf->addrs.name, MAXNAMESIZE);
	sstrncpy(header.cast.info.local_ip, conf->addrs.local_ip, MAXIPV4SIZE);
	sstrncpy(header.cast.info.remote_ip, conf->addrs.remote_ip, MAXIPV4SIZE);
	header.cast.info.local_port = conf->addrs.local_port;
	header.cast.info.remote_port = conf->addrs.remote_port;
	LOGD("CAST header created");
	while (trycount != 0) {
		if (push_broadcast_header(conf->cast_sock, &header, conf->bct_cast) == SUCCESS) {
			trycount = conf->bc_trycount;
			LOGD("CAST header pushed");
		}
		else {
			trycount--;
			LOGD("push_broadcast_header() failed and trycount = %hd", trycount);
		}
		LOGD("sleep for %hds", conf->bc_interval);
		sleep(conf->bc_interval);
	}
	CHECK_STAT(close_socket(conf->cast_sock), "close_socket() failed on socket with fd = %d", conf->cast_sock);
	LOGT("return from start_broadcast()");
	return _stat;
}
