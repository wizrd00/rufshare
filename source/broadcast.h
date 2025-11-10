#include "broadcast.h"

status_t start_broadcast(CntlAddrs *addrs, sockfd_t *sock) {
	status_t _stat = SUCCESS;
	CHECK_STAT(init_udp_socket(sock, addrs->local_ip, addrs->local_port, addrs->remote_ip, &(addrs->remote_port)));
	return _stat;
}
