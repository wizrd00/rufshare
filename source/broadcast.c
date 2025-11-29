#include "broadcast.h"

static status_t push_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout) {
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
			_stat = (pfd.revents & POLLOUT) ? push_udp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf);
			break;
	}
        free(buf);
        return _stat;
}

status_t start_broadcast(void) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	int trycount = conf->bc_trycount;
	CHECK_STAT(init_udp_socket(&conf->cast_sock, conf->addrs.local_ip, conf->addrs.local_port, conf->addrs.remote_ip, conf->addrs.remote_port, true));
	header.cast.packet = pack_RUFShare_CastPacket(0);
	sstrncpy(header.cast.info.name, conf->addrs.name, MAXNAMESIZE);
	sstrncpy(header.cast.info.local_ip, conf->addrs.local_ip, MAXIPV4SIZE);
	sstrncpy(header.cast.info.remote_ip, conf->addrs.remote_ip, MAXIPV4SIZE);
	header.cast.info.local_port = conf->addrs.local_port;
	header.cast.info.remote_port = conf->addrs.remote_port;
	while (trycount != 0) {
		if (push_broadcast_header(conf->cast_sock, &header, conf->bct_cast) == SUCCESS)
			trycount = conf->bc_trycount;
		else
			trycount--;
		sleep(conf->bc_interval);
	}
	return _stat;
}
