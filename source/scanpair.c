#include "scanpair.h"

static status_t pull_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout) {
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
			_stat = (pfd.revents & POLLIN) ? pull_udp_data(sock, (Buffer) &tmp_type, sizeof (RUFShareType)) : ERRPOLL;
			CHECK_STAT(_stat);
			CHECK_EQUAL(CAST, tmp_type, BADTYPE);
			break;
	}
	CHECK_STAT(pull_udp_data(sock, (Buffer) &packet, sizeof (CastPacket)));
	CHECK_STAT(pull_udp_data(sock, (Buffer) infostr, INFOSTRSIZE));
	args->cast.packet = convert_CastPacket_byteorder(&packet);
	unpack_from_infostring(infostr, &(args->cast.info));
	return _stat;
}

status_t start_scanpair(CntlAddrs *addrs, sockfd_t *sock, PairInfo *info, size_t *len, int timeout) {
	status_t _stat = SUCCESS;
	time_t start_time = time(NULL);
	time_t interval = SCANPAIR_INTERVAL;
	CHECK_NOTEQUAL(-1, start_time, ERRTIME);
	HeaderArgs header;
	info = (PairInfo *) malloc(sizeof (PairInfo));
	CHECK_PTR(info, EMALLOC);
	CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, addrs->remote_port, true));
	*len = 1;
	while (time(NULL) - start_time < interval) {
		CHECK_SSTAT(pull_broadcast_header(*sock, &header, timeout), info);
		(*len)++;
		info = realloc(info, sizeof (PairInfo) * (*len));
		CHECK_PTR(info, EMALLOC);
	}
	return _stat;
}
