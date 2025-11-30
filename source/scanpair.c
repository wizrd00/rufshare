#include "scanpair.h"

static status_t network_byteorder(ipv4str_t ip, uint32_t *dst)
{
	status_t _stat = SUCCESS;
	switch (inet_pton(AF_INET, ip, dst)) {
		case -1 :
			_stat = BADARGS;
			break;
		case 0 :
			_stat = BADIPV4;
			break;
	}
	return _stat;
}

static status_t init_udp_socket(sockfd_t *sock, ipv4str_t src_ip, port_t src_port)
{
	status_t _stat = SUCCESS;
	uint32_t src_ipnetorder;
	int tmpsock;
	int optval = 1;
	CHECK_STAT(network_byteorder(src_ip, &src_ipnetorder));
	struct sockaddr_in local_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(src_port),
		.sin_addr = {
			.s_addr = src_ipnetorder
		}
	};
	tmpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	CHECK_INT(tmpsock, INVSOCK);
	*sock = tmpsock;
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (int)), FAILSET);
	CHECK_INT(setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)), FAILSET);
	CHECK_INT(bind(*sock, (struct sockaddr *) &local_addr, sizeof (struct sockaddr_in)), ERRBIND);
	return _stat;
}

static status_t pull_udp_data(sockfd_t sock, buffer_t buf, size_t size)
{
	status_t _stat = SUCCESS;
	ssize_t recv_size = recvfrom(sock, buf, size, 0, NULL, NULL);
	CHECK_INT(recv_size, ERRRECV);
	CHECK_SIZE(recv_size, size);
	return _stat;
}

static status_t pull_broadcast_header(sockfd_t sock, HeaderArgs *args, int timeout)
{
	status_t _stat = SUCCESS;
	CastPacket packet;
	char infostr[INFOSTRSIZE] = {0};
	size_t bufsize = sizeof (CastPacket) + INFOSTRSIZE;
	buffer_t buf = (buffer_t) malloc(bufsize);
	CHECK_PTR(buf, EMALLOC);
	struct pollfd pfd = {.fd = sock, .events = POLLIN};
	switch (poll(&pfd, 1, timeout)) {
		case -1 :
			return _stat = FAILURE;
		case 0 :
			return _stat = TIMEOUT;
		default :
			_stat = (pfd.revents & POLLIN) ? pull_udp_data(sock, buf, bufsize) : ERRPOLL;
			CHECK_SSTAT(_stat, buf);
			break;
	}
	if (ntohs(buf[0]) != CAST)
		CHECK_SSTAT(BADTYPE, buf);
	memcpy((void *) &packet, buf, sizeof (CastPacket));
	memcpy((void *) infostr, buf + sizeof (CastPacket), INFOSTRSIZE);
	args->cast.packet = convert_CastPacket_byteorder(&packet);
	unpack_from_infostring(infostr, &(args->cast.info));
	free(buf);
	return _stat;
}

status_t start_scanpair(PairInfo *info, size_t *len)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	time_t start_time = time(NULL);
	time_t interval = conf->sp_interval;
	int trycount = conf->sp_trycount;
	CHECK_NOTEQUAL(-1, start_time, ERRTIME);
	CHECK_STAT(init_udp_socket(&conf->cast_sock, conf->addrs.local_ip, conf->addrs.local_port));
	*len = 0;
	info = NULL;
	while ((time(NULL) - start_time < interval) || (trycount != 0)) {
		if (pull_broadcast_header(conf->cast_sock, &header, conf->spt_cast) == SUCCESS) {
			trycount = conf->sp_trycount;
			(*len)++;
			info = realloc(info, sizeof (PairInfo) * (*len));
			CHECK_PTR(info, EMALLOC);
			sstrncpy(info[*len - 1].name, header.cast.info.name, MAXNAMESIZE);
			memcpy((void *) info[*len - 1].addr.ip, (void *) header.cast.info.remote_ip, MAXIPV4SIZE);
			info[*len - 1].addr.port = header.cast.info.local_port;
			continue;
		}
		trycount--;
	}
	return _stat;
}
