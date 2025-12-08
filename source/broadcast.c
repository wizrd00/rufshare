#include "broadcast.h"

static status_t push_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	LOGT("in function push_broadcast_header()");
	CHECK_PTR(buf, EMALLOC);
	pack_into_infostring(infostr, &(args->cast.info));
	memcpy((void *) buf, (void *) &(args->cast.packet), sizeof (CastPacket));
	memcpy((void *) buf + sizeof (CastPacket), (void *) infostr, sizeof (infostr));
	LOGD("call poll()");
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
			LOGD("call push_udp_data()");
			_stat = (pfd.revents & POLLOUT) ? push_udp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf);
	}
        free(buf);
	LOGT();
        return _stat;
}

status_t start_broadcast(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	int trycount = conf->bc_trycount;
	LOGT("in function start_broadcast()");
	LOGD("conf->addrs.local_ip = %s", conf->addrs.local_ip);
	LOGD("conf->addrs.local_port = %s", conf->addrs.local_port);
	LOGD("conf->addrs.remote_ip = %s", conf->addrs.remote_ip);
	LOGD("conf->addrs.remote_port = %s", conf->addrs.remote_port);
	CHECK_STAT(init_udp_socket(&conf->cast_sock, conf->addrs.local_ip, conf->addrs.local_port, conf->addrs.remote_ip, conf->addrs.remote_port, true));
	LOGD("UDP socket created with fd = %u", conf->cast_sock);
	header.cast.packet = pack_RUFShare_CastPacket(0);
	sstrncpy(header.cast.info.name, conf->addrs.name, MAXNAMESIZE);
	sstrncpy(header.cast.info.local_ip, conf->addrs.local_ip, MAXIPV4SIZE);
	sstrncpy(header.cast.info.remote_ip, conf->addrs.remote_ip, MAXIPV4SIZE);
	header.cast.info.local_port = conf->addrs.local_port;
	header.cast.info.remote_port = conf->addrs.remote_port;
	LOGT("CAST header created");
	while (trycount != 0) {
		LOGD("call push_broadcast_header() function");
		if (push_broadcast_header(conf->cast_sock, &header, conf->bct_cast) == SUCCESS)
			trycount = conf->bc_trycount;
		else
			trycount--;
		LOGD("trycount = %d", trycount);
		LOGD("sleep for %hd second", conf->bc_interval);
		sleep(conf->bc_interval);
	}
	LOGT("return from start_broadcast()");
	return _stat;
}
