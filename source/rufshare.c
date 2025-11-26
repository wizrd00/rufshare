#include "rufshare.h"

InitConfig *conf;

#ifdef LOGGING
static void *thread_start_logd(void *arg) {
	start_logd();
	return arg;
}
#endif

static status_t start_logging(pthread_t *handle) {
	status_t _stat = SUCCESS;
	#ifdef LOGGING
	CHECK_THREAD(pthread_create(handle, NULL, thread_start_logd, NULL));
	#else
	*handle = 0;
	#endif
	return _stat;
}

static char *sstrncpy(char *dst, const char *src, size_t dsize) {
	strncpy(dst, src, dsize - 1);
		dst[dsize - 1] = '\0';
	return dst;
}

static void set_global_variables(HeaderArgs *header) {
	conf->chsize = header->send.packet.chunk_size;
	conf->pchsize = header->send.packet.partial_chunk_size;
	conf->chcount = header->send.packet.chunk_count;
	return;
}

static bool match_flow(HeaderArgs *header) {
	return ((header->flow.packet.sequence != conf->seq) || (header->flow.packet.chunk_size != conf->chsize)) ? false : true;
}

static void *thread_calc_file_crc16(void *arg) {
	RUFShareCRC16 *crc = (RUFShareCRC16 *) arg;
	*crc = calc_file_crc16(&conf->filec);
	return arg;
}

static void *thread_start_broadcast(void *arg) {
	start_broadcast((CntlAddrs *) arg, &conf->cast_sock);
	return arg;
}

static status_t push_handshake(const char *name) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	LOGT(__FILE__, __func__, "start handshake");
	chunk_count = calc_chunk_count(conf->filec.size, conf.chsize, &conf.pchsize);
	LOGD(__FILE__, __func__, "file with size %lu, splitted into %lu chunks with size %lu", conf->filec.size, conf->chcount, conf->chsize);
	header.send.packet = pack_RUFShare_SendPacket(conf->chsize, conf->chcount, conf->pchsize, 0);
	header.send.info = addrs;
	CHECK_STAT(push_SEND_header(conf->cntl_sock, &header, HANDSHAKE_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pushed on socket fd %d with %d timeout", conf->cntl_sock, HANDSHAKE_SEND_TIMEOUT);
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, HANDSHAKE_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pulled on socket fd %d with %d timeout", conf->cntl_sock, HANDSHAKE_RECV_TIMEOUT);
	if (header.recv.packet.ack == 0)
		_stat = ZEROACK;
	LOGD(__FILE__, __func__, "header.recv.packet.ack = %d", header.recv.packet.ack);
	LOGT(__FILE__, __func__, "handshake complete");
	return _stat;
}

static status_t pull_handshake(const char *path) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	LOGT(__FILE__, __func__, "start handshake");
	CHECK_STAT(pull_SEND_header(conf->conn_sock, &header, FOREVER_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pulled on socket fd %d with %d timeout", conf->conn_sock, FOREVER_TIMEOUT);
	set_global_variables(&header);
	sstrncpy(conf->filec.name, header.send.info.filename, MAXFILENAMESIZE);
	sstrncpy(conf->addrs.filename, header.send.info.filename, MAXFILENAMESIZE);
	LOGD(__FILE__, __func__, "filename %s copied into filec.name and addrs.filename", conf->addrs.filename);
	LOGT(__FILE__, __func__, "name of file is \"%s\"", conf->addrs.filename);
	sstrncpy(conf->addrs.name, header.send.info.name, MAXNAMESIZE);
	LOGD(__FILE__, __func__, "name %s copied into addrs.name", conf->addrs.name);
	LOGT(__FILE__, __func__, "remote name is \"%s\"", conf->addrs.name);
	sstrncpy(conf->addrs.remote_ip, header.send.info.local_ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "remote_ip %s copied into addrs.remote_ip", conf->addrs.remote_ip);
	LOGT(__FILE__, __func__, "remote ip is \"%s\"", conf->addrs.remote_ip);
	conf->addrs.remote_port = header.send.info.local_port;
	LOGD(__FILE__, __func__, "remote_port %s assigned into addrs.remote_port", conf->addrs.remote_port);
	LOGT(__FILE__, __func__, "remote port is \"%hu\"", conf->addrs.remote_port);
	conf->filec.size = calc_file_size(conf->chcount, conf->chsize, conf->pchsize);
	LOGD(__FILE__, __func__, "%lu chunks with size %lu, merged into file with size %lu", conf->chcount, conf->chsize, conf->filec.size);
	LOGT(__FILE__, __func__, "size of the file is %lu", conf->filec.size);
	CHECK_NOTEQUAL(0, conf->filec.size, LOWSIZE);
	LOGD(__FILE__, __func__, "call start_file_stream()");
	header.recv.packet = pack_RUFShare_RecvPacket((start_file_stream(&conf->filec, path, MWR) == SUCCESS) ? 1 : 0, 0, 0);
	CHECK_STAT(push_RECV_header(conf->conn_sock, &header, HANDSHAKE_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout", conf->conn_sock, HANDSHAKE_RECV_TIMEOUT);
	LOGT(__FILE__, __func__, "handshake complete");
	return _stat;
}

static status_t push_transfer(void) {
	status_t _stat = SUCCESS;
	int trycount = TRANSFER_TRY_COUNT;
	ChunkContext chcon;
	HeaderArgs header0;
	HeaderArgs header1;
	RUFShareCRC32 crc;
	LOGT(__FILE__, __func__, "start transfer");
	CHECK_EQUAL(0, conf->seq, ZEROSEQ);
	while (conf->seq <= conf->chcount) {
		chcon.start_pos = (conf->seq - 1) * conf->chsize;
		chcon.chunk_size = (conf->seq == conf->chcount) ? conf->pchsize : conf->chsize;
		crc = calc_chunk_crc32(&conf->filec, &chcon);
		header0.flow.packet = pack_RUFShare_FlowPacket(chcon.chunk_size, seq, crc);
		LOGD(__FILE__, __func__, "start_pos = %lu, chunk_size = %lu, crc = %lu", chcon.start_pos, chcon.chunk_size, crc);
		while (trycount != 0) {
			LOGD(__FILE__, __func__, "%d tries remained in control section");
			LOGD(__FILE__, __func__, "sending control headers");
			CHECK_STAT(push_FLOW_header(conf->cntl_sock, &header0, TRANSFER_FLOW_TIMEOUT));
			CHECK_STAT(pull_RECV_header(conf->cntl_sock, &header1, TRANSFER_RECV_TIMEOUT));
			if (header1.recv.packet.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY0);
		trycount = TRANSFER_TRY_COUNT;
		while (trycount != 0) {
			LOGD(__FILE__, __func__, "%d tries remained in data section");
			LOGD(__FILE__, __func__, "sending chunk data with size %lu", chcon.chunk_size);
			CHECK_STAT(push_chunk_data(conf->data_sock, &conf->filec, &chcon, TRANSFER_DATA_TIMEOUT));
			CHECK_STAT(pull_RECV_header(conf->cntl_sock, &header1, TRANSFER_RECV_TIMEOUT));
			if (header1.recv.packet.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY1);
		trycount = TRANSFER_TRY_COUNT;
		conf->seq++;
	}
	LOGT(__FILE__, __func__, "transfer complete");
	return _stat;
}

static status_t pull_transfer(void) {
	status_t _stat = SUCCESS;
	ChunkContext chcon;
	HeaderArgs header0;
	HeaderArgs header1;
	RUFShareCRC32 crc;
	int trycount = TRANSFER_TRY_COUNT;
	LOGT(__FILE__, __func__, "start transfer");
	CHECK_EQUAL(0, conf->seq, ZEROSEQ);
	while (conf->seq <= conf->chcount) {
		chcon.start_pos = (conf->seq - 1) * conf->chsize;
		chcon.chunk_size = (conf->seq == conf->chcount) ? conf->pchsize : conf->chsize;
		LOGD(__FILE__, __func__, "start_pos = %lu, chunk_size = %lu", chcon.start_pos, chcon.chunk_size);
		CHECK_STAT(pull_FLOW_header(conf->conn_sock, &header0, TRANSFER_FLOW_TIMEOUT));
		LOGD(__FILE__, __func__, "FLOW packet pulled on socket fd %d with %d timeout", conf->conn_sock, TRANSFER_FLOW_TIMEOUT);
		if (match_flow(&header0)) {
			LOGD(__FILE__, __func__, "FLOW packet important fields matched");
			header1.recv.packet = pack_RUFShare_RecvPacket(1, 0, conf->seq);
			CHECK_STAT(push_RECV_header(conf->conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conf->conn_sock, TRANSFER_FLOW_TIMEOUT, header1.recv.packet.ack);
			while (trycount != 0) {
				LOGD(__FILE__, __func__, "%d tries remained");
				LOGD(__FILE__, __func__, "receiving chunk data with size %lu", chcon.chunk_size);
				CHECK_STAT(pull_chunk_data(conf->data_sock, &conf->filec, &chcon, TRANSFER_DATA_TIMEOUT));	
				crc = calc_chunk_crc32(&conf->filec, &chcon);
				if (header0.flow.packet.crc == crc) {
					LOGD(__FILE__, __func__, "FLOW crc == chunk crc");
					header1.recv.packet = pack_RUFShare_RecvPacket(1, 0, conf->seq);
					CHECK_STAT(push_RECV_header(conf->conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conf->conn_sock, TRANSFER_RECV_TIMEOUT, header1.recv.packet.ack);
					break;
				}
				else {
					LOGD(__FILE__, __func__, "FLOW crc != chunk crc");
					header1.recv.packet = pack_RUFShare_RecvPacket(0, 0, conf->seq);
					CHECK_STAT(push_RECV_header(conf->conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conf->conn_sock, TRANSFER_RECV_TIMEOUT, header1.recv.packet.ack);
					trycount--;
				}
			}
			CHECK_NOTEQUAL(0, trycount, EXPTRY0);
			trycount = TRANSFER_TRY_COUNT;
			conf->seq++;
		}
		else {
			LOGD(__FILE__, __func__, "FLOW packet important fields not matched");
			header1.recv.packet = pack_RUFShare_RecvPacket(0, 0, conf->seq);
			push_RECV_header(conf->conn_sock, &header1, TRANSFER_RECV_TIMEOUT);
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conf->conn_sock, TRANSFER_RECV_TIMEOUT, header1.recv.packet.ack);
			_stat = BADFLOW;
			break;
		}
	}
	LOGT(__FILE__, __func__, "transfer complete");
	return _stat;
}

static status_t push_verification(void) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	LOGT(__FILE__, __func__, "start verification");
	RUFShareCRC16 crc = calc_file_crc16(&conf->filec);
	header.send.packet = pack_RUFShare_SendPacket(conf->chsize, conf->chcount, conf->pchsize, crc);
	header.send.info = conf->addrs;
	CHECK_STAT(push_SEND_header(conf->cntl_sock, &header, VERIFICATION_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pushed on socket fd %d with timeout %d", conf->cntl_sock, VERIFICATION_SEND_TIMEOUT);
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, VERIFICATION_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pulled on socket fd %d with %d timeout", conf->cntl_sock, VERIFICATION_RECV_TIMEOUT);
	if (header.recv.packet.ack == 0)
		_stat = ZEROACK;
	LOGD(__FILE__, __func__, "header.recv.packet.ack = %d", header.recv.packet.ack);
	LOGT(__FILE__, __func__, "verification complete");
	return _stat;
}

static status_t pull_verification(void) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	RUFShareCRC16 crc;
	pthread_t handle;
	LOGT(__FILE__, __func__, "start verification");
	CHECK_THREAD(pthread_create(&handle, NULL, thread_calc_file_crc16, (void *) &crc));
	CHECK_STAT(pull_SEND_header(conf->conn_sock, &header, VERIFICATION_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pulled on socket fd %d with timeout %d", conf->conn_sock, VERIFICATION_SEND_TIMEOUT);
	LOGD(__FILE__, __func__, "wait for crc calculation thread to join");
	CHECK_THREAD(pthread_join(handle, NULL));
	header.recv.packet = pack_RUFShare_RecvPacket((header.send.packet.crc == crc) ? 1 : 0, 0, conf->seq); 
	CHECK_STAT(push_RECV_header(conf->conn_sock, &header, VERIFICATION_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conf->conn_sock, VERIFICATION_RECV_TIMEOUT, header.recv.packet.ack);
	if (header.send.packet.crc != crc) {
		_stat = FAILCRC;
		LOGT(__FILE__, __func__, "remote file crc does not match to local file crc");
	}
	LOGT(__FILE__, __func__, "verification complete");
	return _stat;
}

status_t push_file(const char *name, const char *path, addr_pair *local, addr_pair *remote, size_t chunk_size) {
	status_t _stat = SUCCESS;
	pthread_t handle;
	conf->chsize = (RUFShareChunkSize) chunk_size;
	CHECK_NOTEQUAL(0, chunk_size, ZEROCHK);
	LOGT(__FILE__, __func__, "start push_file with name %s", name);
	CHECK_STAT(start_logging(&handle));
	extract_file_name(conf->addrs.filename, path, MAXFILENAMESIZE);
	LOGD(__FILE__, __func__, "push_file() : filename = %s", conf->addrs.filename);
	sstrncpy(conf->addrs.name, name, MAXNAMESIZE);
	LOGD(__FILE__, __func__, "push_file() : name = %s", conf->addrs.name);
	sstrncpy(conf->addrs.local_ip, local->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "push_file() : local_ip = %s", conf->addrs.local_ip);
	sstrncpy(conf->addrs.remote_ip, remote->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "push_file() : remote_ip = %s", conf->addrs.remote_ip);
	conf->addrs.local_port = local->port;
	addrs.remote_port = remote->port;
	LOGD(__FILE__, __func__, "push_file() : local_port = %hu & remote_port = %hu", conf->addrs.local_port, conf->addrs.remote_port);
	LOGD(__FILE__, __func__, "call start_file_stream()");
	tryexec(start_file_stream(&conf->filec, path, MRD));
	LOGD(__FILE__, __func__, "call start_cntl() with conn = true");
	tryexec(start_cntl(&conf->addrs, &conf->cntl_sock, true));
	LOGD(__FILE__, __func__, "call push_handshake()");
	tryexec(push_handshake(name));
	LOGD(__FILE__, __func__, "call start_data()");
	tryexec(start_data(&conf->addrs, &conf->data_sock));
	LOGD(__FILE__, __func__, "call push_transfer()");
	tryexec(push_transfer()); LOGD(__FILE__, __func__, "call push_verification()");
	tryexec(push_verification());
	LOGT(__FILE__, __func__, "end push_file() with name %s", conf->addrs.name);
	tryexec(end_file_stream(&conf->filec));
	tryexec(end_cntl(conf->cntl_sock));
	tryexec(end_data(conf->data_sock));
	return _stat;
}

status_t pull_file(const char *name, const char *path, addr_pair *local, addr_pair *remote) {
	status_t _stat = SUCCESS;
	CntlAddrs broadcast_addrs = {.local_port = conf->addrs.local_port, .remote_port = conf->addrs.remote_port};
	pthread_t handle[2];
	LOGT(__FILE__, __func__, "start pull_file with name %s", name);
	CHECK_STAT(start_logging(&handle[0]));
	sstrncpy(broadcast_addrs.filename , name, MAXFILENAMESIZE); 
	sstrncpy(broadcast_addrs.name , name, MAXNAMESIZE); 
	sstrncpy(broadcast_addrs.local_ip, local->ip, MAXIPV4SIZE);
	sstrncpy(broadcast_addrs.remote_ip, BROADCAST_IPV4, MAXIPV4SIZE);
	sstrncpy(conf->addrs.name, name, MAXNAMESIZE);
	broadcast_addrs.local_port = local->port;
	broadcast_addrs.remote_port = BROADCAST_PORT;
	LOGD(__FILE__, __func__, "pull_file() : name = %s", conf->addrs.name);
	sstrncpy(conf->addrs.local_ip, local->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "pull_file() : local_ip = %s", conf->addrs.local_ip);
	sstrncpy(conf->addrs.remote_ip, remote->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "pull_file() : remote_ip = %s", conf->addrs.remote_ip);
	conf->addrs.local_port = local->port;
	conf->addrs.remote_port = remote->port;
	LOGD(__FILE__, __func__, "pull_file() : local_port = %hu & remote_port = %hu", conf->addrs.local_port, conf->addrs.remote_port);
	LOGD(__FILE__, __func__, "call start_cntl() with conn = false");
	tryexec(start_cntl(&conf->addrs, &conf->cntl_sock, false));
	CHECK_THREAD(pthread_create(&handle[1], NULL, thread_start_broadcast, (void *) &broadcast_addrs));
	LOGD(__FILE__, __func__, "call accept_cntl()");
	tryexec(accept_cntl(&conf->addrs, &conf->conn_sock, conf->cntl_sock, FOREVER_TIMEOUT));
	CHECK_THREAD(pthread_cancel(handle[1]));
	LOGD(__FILE__, __func__, "call pull_handshake()");
	tryexec(pull_handshake(path));
	LOGD(__FILE__, __func__, "call start_data()");
	tryexec(start_data(&conf->addrs, &conf->data_sock));
	LOGD(__FILE__, __func__, "call transfer()");
	tryexec(pull_transfer());
	LOGD(__FILE__, __func__, "call pull_verification()");
	tryexec(pull_verification());
	LOGT(__FILE__, __func__, "end pull_file() with name %s", conf->addrs.name);
	tryexec(end_file_stream(&conf->filec));
	tryexec(end_data(conf->cast_sock));
	tryexec(end_cntl(conf->cntl_sock));
	tryexec(end_cntl(conf->conn_sock));
	tryexec(end_data(conf->data_sock));
	return _stat;
}

status_t scan_pair(PairInfo *info, size_t *len, addr_pair *local) {
	status_t _stat = SUCCESS;
	CntlAddrs broadcast_addrs = {
		.local_port = local->port,
		.remote_port = BROADCAST_PORT
	};
	pthread_t handle;
	CHECK_STAT(start_logging(&handle));
	sstrncpy(broadcast_addrs.local_ip, local->ip, MAXIPV4SIZE);
	sstrncpy(broadcast_addrs.remote_ip, BROADCAST_IPV4, MAXIPV4SIZE);
	tryexec(start_scanpair(&broadcast_addrs, &conf->cast_sock, info, len, SCANPAIR_TIMEOUT));
	return _stat;
}

status_t initiate(InitConfig *initconf) {
	status_t _stat = SUCCESS;
	CHECK_NOTEQUAL(0, initconf->chsize, BADCONF);
	CHECK_NOTEQUAL(0, initconf->addrs.name[0], BADCONF);
	CHECK_NOTEQUAL(0, initconf->addrs.local_ip[0], BADCONF);
	CHECK_NOTEQUAL(0, initconf->addrs.local_port[0], BADCONF);
	CHECK_NOTEQUAL(0, initconf->segsize, BADCONF);
	if (initconf->segsize > initconf->chsize)
		return _stat = BADCONF;
	if ((initconf->sp_interval <= 0) || (initconf->bc_interval <= 0))
		return _stat = BADCONF;
	if ((initconf->tf_trycount <= 0) || (initconf->bc_trycount <= 0))
		return _stat = BADCONF;
	conf = initconf;
	// TODO
	return _stat;
}
