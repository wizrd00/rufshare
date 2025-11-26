#include "broadcast.h"

status_t push_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout) {
	status_t _stat = SUCCESS;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLOUT};
	CHECK_PTR(buf, EMALLOC);
	pack_into_infostring(infostr, &(args->cast.info));
	memcpy(buf, &(args->cast.packet), sizeof (CastPacket));
	memcpy(buf + sizeof (CastPacket), infostr, sizeof (infostr));
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

status_t start_broadcast(CntlAddrs *addrs, sockfd_t *sock) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	int trycount = BROADCAST_TRY_COUNT;
	CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port, true));
	header.cast.packet = pack_RUFShare_CastPacket(0);
	header.cast.info = *addrs;
	while (1) {
		while (trycount != 0) {
			if (push_broadcast_header(*sock, &header, BROADCAST_CAST_TIMEOUT) != SUCCESS)
				trycount--;
			sleep(BROADCAST_INTERVAL);
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY0);
	}
	return _stat;
}
