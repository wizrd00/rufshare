#include "scanpair.h"

static status_t network_byteorder(ipv4str_t ip, uint32_t *dst)
{
	status_t _stat = SUCCESS;
	LOGT("in function network_byteorder()");
	switch (inet_pton(AF_INET, ip, dst)) {
		case -1 :
			CHECK_STAT(BADARGS, "invalid address family");
		case 0 :
			CHECK_STAT(BADIPV4, "invalid ip address");
	}
	LOGT("return from network_byteorder()");
	return _stat;
}

static status_t init_udp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port)
{
	status_t _stat = SUCCESS;
	uint32_t src_ipnetorder;
	int tmpsock;
	int optval = 1;
	LOGT("in function init_udp_socket()");
	CHECK_STAT(network_byteorder(src_ip, &src_ipnetorder), "network_byteorder() failed");
	LOGD("ip addresses converted into network byteorder");
	struct sockaddr_in local_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(src_port),
		.sin_addr = {
			.s_addr = src_ipnetorder
		}
	};
	LOGD("create UDP datagram socket with address family AF_INET");
	tmpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	CHECK_INT(tmpsock, INVSOCK);
	*sock = tmpsock;
	LOGD("set SO_BROADCAST option on socket with fd = %d", *sock);
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (int)), FAILSET, "setsockopt() failed to set SO_BROADCAST option on socket with fd = %d", *sock);
	LOGD("set SO_REUSEADDR option on socket with fd = %d", *sock);
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET, "setsockopt() failed to set SO_RUSEADDR option on socket with fd = %d", *sock);
	LOGD("bind to local address %s:%hu on socket with fd = %d", src_ip, src_port, *sock);
	CHECK_INT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND, "bind() failed on socket with fd = %d", *sock);
	LOGT("return from init_udp_socket()");
	return _stat;
}

static status_t pull_udp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	LOGT("in function pull_udp_data()");
	LOGD("recv buffer with size %zu on socket with fd = %d", size, sock);
	ssize_t recv_size = recvfrom(sock, buf, size, 0, NULL, NULL);
	CHECK_INT(recv_size, ERRRECV, "recvfrom() failed on socket with fd = %d", sock);
	CHECK_SIZE(recv_size, size, "recvfrom() pulled less than expected size %zu bytes", size);
	LOGT("return from pull_udp_data()");
	return _stat;
}

static status_t pull_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	CastPacket packet;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	LOGT("in function pull_broadcast_header()");
	CHECK_PTR(buf, EMALLOC, "malloc() failed to allocate buffer with size = %zu", bufsize);
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			free(buf);
			return _stat = FAILURE;
		case 0 :
			free(buf);
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLIN) ? pull_udp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf, "pull_udp_data() failed to pull CAST packet on socket with fd = %d", sock);
			LOGD("CAST packet pulled");
	}
	LOGD("check packet type == CAST");
	if (ntohs(buf[0]) != CAST)
		CHECK_SSTAT(BADTYPE, buf, "invalid type != CAST");
	memcpy((void *) &packet, buf, sizeof (CastPacket));
	memcpy((void *) infostr, buf + sizeof (CastPacket), INFOSTRSIZE);
	args->cast.packet = convert_CastPacket_byteorder(&packet);
	unpack_from_infostring(infostr, &(args->cast.info));
	LOGD("CAST packet unpacked");
	free(buf);
	LOGT("return from pull_broadcast_header()");
	return _stat;
}

status_t start_scanpair(PairInfo *info, size_t *len)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	time_t start_time = time(NULL);
	time_t interval = conf->sp_interval;
	int trycount = conf->sp_trycount;
	LOGT("in function start_scanpair()");
	CHECK_NOTEQUAL(-1, start_time, ERRTIME, "time() failed");
	CHECK_STAT(init_udp_socket(&conf->cast_sock, conf->addrs.local_ip, conf->addrs.local_port), "init_udp_socket() failed");
	LOGD("UDP socket created with fd = %d", conf->cast_sock);
	LOGD("set *len = 0, info = NULL");
	*len = 0;
	info = NULL;
	while ((time(NULL) - start_time < interval) || (trycount != 0)) {
		if (pull_broadcast_header(conf->cast_sock, &header, conf->spt_cast) == SUCCESS) {
			LOGD("CAST packet pulled");
			trycount = conf->sp_trycount;
			(*len)++;
			info = realloc(info, sizeof (PairInfo) * (*len));
			CHECK_PTR(info, EMALLOC, "realloc() failed to reallocate buffer with size = %zu", sizeof (PairInfo) * (*len));
			sstrncpy(info[*len - 1].name, header.cast.info.name, MAXNAMESIZE);
			memcpy((void *) info[*len - 1].addr.ip, (void *) header.cast.info.remote_ip, MAXIPV4SIZE);
			info[*len - 1].addr.port = header.cast.info.local_port;
			continue;
		}
		trycount--;
	}
	LOGT("return from start_scanpair()");
	return _stat;
}
