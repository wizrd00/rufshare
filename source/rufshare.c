#include "rufshare.h"

RUFShareChunkSize chunk_size;
RUFSharePartialChunkSize partial_chunk_size;
RUFShareChunkCount chunk_count;
sockfd_t cntl_sock;
sockfd_t data_sock;
sockfd_t conn_sock;
sockfd_t cast_sock;
FileContext filec;
CntlAddrs addrs;

static void set_global_variables(HeaderArgs *header) {
	chunk_size = header->send.packet.chunk_size;
	partial_chunk_size = header->send.packet.partial_chunk_size;
	chunk_count = header->send.packet.chunk_count;
	return;
}

static bool match_flow(HeaderArgs *header, RUFShareSequence *seq, RUFShareCRC32 crc) {
	return ((header->flow.packet.sequence != *seq) || (header->flow.packet.chunk_size != chunk_size)) ? false : true;
}

static void thread_calc_file_crc16(void *arg) {
	RUFShareCRC16 *crc = (RUFShareCRC16 *) arg;
	*crc = calc_file_crc16(&filec);
	return;
}

static void thread_start_broadcast(void *arg) {
	start_broadcast((CntlAddrs *) arg, &cast_sock);
	return;
}

static status_t push_handshake(const char *name) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	LOGT(__FILE__, __func__, "start handshake");
	chunk_count = calc_chunk_count(filec.size, chunk_size, &partial_chunk_size);
	LOGD(__FILE__, __func__, "file with size %lu, splitted into %lu chunks with size %lu", filec.size, chunk_count, chunk_size);
	header.send.packet = pack_RUFShare_SendPacket(chunk_size, chunk_count, partial_chunk_size, 0);
	header.send.info = addrs;
	CHECK_STAT(push_SEND_header(cntl_sock, &header, HANDSHAKE_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pushed on socket fd %d with %d timeout", cntl_sock, HANDSHAKE_SEND_TIMEOUT);
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, HANDSHAKE_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pulled on socket fd %d with %d timeout", cntl_sock, HANDSHAKE_RECV_TIMEOUT);
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
	CHECK_STAT(pull_SEND_header(conn_sock, &header, FOREVER_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pulled on socket fd %d with %d timeout", conn_sock, FOREVER_TIMEOUT);
	strncpy(filec.name, header.send.info.filename, MAXFILENAMESIZE);
	strncpy(addrs.filename, header.send.info.filename, MAXFILENAMESIZE);
	LOGD(__FILE__, __func__, "filename %s copied into filec.name and addrs.filename", addrs.filename);
	LOGT(__FILE__, __func__, "name of file is \"%s\"", addrs.filename);
	strncpy(addrs.name, header.send.info.name, MAXNAMESIZE);
	LOGD(__FILE__, __func__, "name %s copied into addrs.name", addrs.name);
	LOGT(__FILE__, __func__, "remote name is \"%s\"", addrs.name);
	strncpy(addrs.remote_ip, header.send.info.local_ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "remote_ip %s copied into addrs.remote_ip", addrs.remote_ip);
	LOGT(__FILE__, __func__, "remote ip is \"%s\"", addrs.remote_ip);
	addrs.remote_port = header.send.info.local_port;
	LOGD(__FILE__, __func__, "remote_port %s assigned into addrs.remote_port", addrs.remote_port);
	LOGT(__FILE__, __func__, "remote port is \"%hu\"", addrs.remote_port);
	filec.size = calc_file_size(chunk_count, chunk_size, partial_chunk_size);
	LOGD(__FILE__, __func__, "%lu chunks with size %lu, merged into file with size %lu", chunk_count, chunk_size, filec.size);
	LOGT(__FILE__, __func__, "size of the file is %lu", filec.size);
	CHECK_NOTEQUAL(0, filec.size, LOWSIZE);
	LOGD(__FILE__, __func__, "call start_file_stream()");
	header.recv.packet = pack_RUFShare_RecvPacket((start_file_stream(&filec, path, MWR) == SUCCESS) ? 1 : 0, 0, 0);
	CHECK_STAT(push_RECV_header(conn_sock, &header, HANDSHAKE_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout", conn_sock, HANDSHAKE_RECV_TIMEOUT);
	LOGT(__FILE__, __func__, "handshake complete");
	return _stat;
}

static status_t push_transfer(RUFShareSequence *seq) {
	status_t _stat = SUCCESS;
	int trycount = TRANSFER_TRY_COUNT;
	ChunkContext chcon;
	HeaderArgs header0;
	HeaderArgs header1;
	RUFShareCRC32 crc;
	LOGT(__FILE__, __func__, "start transfer");
	CHECK_EQUAL(0, *seq, ZEROSEQ);
	while (*seq <= chunk_count) {
		chcon.start_pos = (*seq - 1) * chunk_size;
		chcon.chunk_size = (*seq == chunk_count) ? partial_chunk_size : chunk_size;
		crc = calc_chunk_crc32(&filec, &chcon);
		header.flow.packet = pack_FLOW_FlowPacket(((*seq == chunk_count) ? partial_chunk_size : chunk_size), *seq, crc);
		LOGD(__FILE__, __func__, "start_pos = %lu, chunk_size = %lu, crc = %lu", chcon.start_pos, chcon.chunk_size, crc);
		while (trycount != 0) {
			LOGD(__FILE__, __func__, "%d tries remained in control section");
			LOGD(__FILE__, __func__, "sending control headers");
			CHECK_STAT(push_FLOW_header(cntl_sock, &header0, TRANSFER_FLOW_TIMEOUT));
			CHECK_STAT(pull_RECV_header(cntl_sock, &header1, TRANSFER_RECV_TIMEOUT));
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
			CHECK_STAT(push_chunk_data(data_sock, &filec, &chcon, TRANSFER_DATA_TIMEOUT));
			CHECK_STAT(pull_RECV_header(cntl_sock, &header1, TRANSFER_RECV_TIMEOUT));
			if (header1.recv.packet.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY1);
		trycount = TRANSFER_TRY_COUNT;
		(*seq)++;
	}
	LOGT(__FILE__, __func__, "transfer complete");
	return _stat;
}

static status_t pull_transfer(RUFShareSequence *seq) {
	status_t _stat = SUCCESS;
	ChunkContext chcon;
	HeaderArgs header0;
	HeaderArgs header1;
	RUFShareCRC32 crc;
	int trycount = TRANSFER_TRY_COUNT;
	LOGT(__FILE__, __func__, "start transfer");
	CHECK_EQUAL(0, *seq, ZEROSEQ);
	while (*seq <= chunk_count) {
		chcon.start_pos = (*seq - 1) * chunk_size;
		chcon.chunk_size = (*seq == chunk_count) ? partial_chunk_size : chunk_size;
		LOGD(__FILE__, __func__, "start_pos = %lu, chunk_size = %lu", chcon.start_pos, chcon.chunk_size);
		CHECK_STAT(pull_FLOW_header(conn_sock, &header0, TRANSFER_FLOW_TIMEOUT));
		LOGD(__FILE__, __func__, "FLOW packet pulled on socket fd %d with %d timeout", conn_sock, TRANSFER_FLOW_TIMEOUT);
		if (match_flow(&header0, seq)) {
			LOGD(__FILE__, __func__, "FLOW packet important fields matched");
			header1.recv.packet = pack_RUFShare_RecvPacket(1, 0, *seq);
			CHECK_STAT(push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conn_sock, TRANSFER_FLOW_TIMEOUT, header1.recv.packet.ack);
			while (trycount != 0) {
				LOGD(__FILE__, __func__, "%d tries remained");
				LOGD(__FILE__, __func__, "receiving chunk data with size %lu", chcon.chunk_size);
				CHECK_STAT(pull_chunk_data(data_sock, &filec, &chcon, TRANSFER_DATA_TIMEOUT));	
				crc = calc_chunk_crc32(&filec, &chcon);
				if (header0.flow.packet.crc == crc) {
					LOGD(__FILE__, __func__, "FLOW crc == chunk crc");
					header1.recv.packet = pack_RUFShare_RecvPacket(1, 0, *seq);
					CHECK_STAT(push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conn_sock, TRANSFER_RECV_TIMEOUT, header1.recv.packet.ack);
					break;
				}
				else {
					LOGD(__FILE__, __func__, "FLOW crc != chunk crc");
					header1.recv.packet = pack_RUFShare_RecvPacket(0, 0, *seq);
					CHECK_STAT(push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conn_sock, TRANSFER_RECV_TIMEOUT, header1.recv.packet.ack);
					trycount--;
				}
			}
			CHECK_NOTEQUAL(0, trycount, EXPTRY0);
			trycount = TRANSFER_TRY_COUNT;
			(*seq)++;
		}
		else {
			LOGD(__FILE__, __func__, "FLOW packet important fields not matched");
			header1.recv.packet = pack_RUFShare_RecvPacket(0, 0, *seq);
			push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT);
			LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conn_sock, TRANSFER_RECV_TIMEOUT, header1.recv.packet.ack);
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
	RUFShareCRC16 crc = calc_file_crc16(&filec);
	header.send.packet = pack_RUFShare_SendPacket(chunk_size, chunk_count, partial_chunk_size, crc);
	header.send.info = addrs;
	CHECK_STAT(push_SEND_header(cntl_sock, &header, VERIFICATION_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pushed on socket fd %d with timeout %d", cntl_sock, VERIFICATION_SEND_TIMEOUT);
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, VERIFICATION_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pulled on socket fd %d with %d timeout", cntl_sock, VERIFICATION_RECV_TIMEOUT);
	if (header.recv.packet.ack == 0)
		_stat = ZEROACK;
	LOGD(__FILE__, __func__, "header.recv.packet.ack = %d", header.recv.packet.ack);
	LOGT(__FILE__, __func__, "verification complete");
	return _stat;
}

static status_t pull_verification(RUFShareSequence *seq) {
	status_t _stat = SUCCESS;
	HeaderArgs header;
	RUFShareCRC16 crc;
	pthread_t handle;
	LOGT(__FILE__, __func__, "start verification");
	CHECK_THREAD(pthread_create(&handle, NULL, thread_calc_file_crc16, (void *) &crc));
	CHECK_STAT(pull_SEND_header(conn_sock, &header, VERIFICATION_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pulled on socket fd %d with timeout %d", conn_sock, VERIFICATION_SEND_TIMEOUT);
	LOGD(__FILE__, __func__, "wait for crc calculation thread to join");
	CHECK_THREAD(pthread_join(&handle, NULL));
	header.recv.packet = pack_RUFShare_RecvPacket((header.send.packet.crc == crc) ? 1 : 0, 0, *seq); 
	CHECK_STAT(push_RECV_header(conn_sock, &header, VERIFICATION_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pushed on socket fd %d with %d timeout and ack = %d", conn_sock, VERIFICATION_RECV_TIMEOUT, header.recv.packet.ack);
	if (header.send.packet.crc != crc) {
		_stat = FAILCRC;
		LOGT(__FILE__, __func__, "remote file crc does not match to local file crc");
	}
	LOGT(__FILE__, __func__, "verification complete");
	return _stat;
}

status_t push_file(const char *name, const char *path, addr_pair *local, addr_pair *remote) {
	status_t _stat = SUCCESS;
	RUFShareSequence seq = 1;
	LOGT(__FILE__, __func__, "start push_file with name %s", name);
	extract_file_name(addrs.filename, path, MAXFILENAMESIZE);
	LOGD(__FILE__, __func__, "push_file() : filename = %s", addrs.filename);
	strncpy(addrs.name, name, MAXNAMESIZE);
	LOGD(__FILE__, __func__, "push_file() : name = %s", addrs.name);
	strncpy(addrs.local_ip, local->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "push_file() : local_ip = %s", addrs.local_ip);
	strncpy(addrs.remote_ip, remote->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "push_file() : remote_ip = %s", addrs.remote_ip);
	addrs.local_port = local->port;
	addrs.remote_port = remote->port;
	LOGD(__FILE__, __func__, "push_file() : local_port = %hu & remote_port = %hu", addrs.local_port, addrs.remote_port);
	LOGD(__FILE__, __func__, "call start_file_stream()");
	tryexec(start_file_stream(&filec, path, MRD));
	LOGD(__FILE__, __func__, "call start_cntl() with conn = true");
	tryexec(start_cntl(&addrs, &cntl_sock, true));
	LOGD(__FILE__, __func__, "call push_handshake()");
	tryexec(handshake());
	LOGD(__FILE__, __func__, "call start_data()");
	tryexec(start_data(&addrs, &data_sock));
	LOGD(__FILE__, __func__, "call push_transfer()");
	tryexec(transfer(&seq));
	LOGD(__FILE__, __func__, "call push_verification()");
	tryexec(verification());
	LOGT(__FILE__, __func__, "end push_file() with name %s", addrs.name):
	tryexec(end_file_stream(&filec));
	tryexec(end_cntl(cntl_sock));
	tryexec(end_data(data_sock));
	return _stat;
}

status_t pull_file(const char *name, const char *path, addr_pair *local, addr_pair *remote) {
	status_t _stat = SUCCESS;
	RUFShareSequence seq = 1;
	CntlAddrs broadcast_addrs = {.local_port = addrs.local_port, .remote_port = addrs.remote_port};
	pthread_t handle;
	LOGT(__FILE__, __func__, "start pull_file with name %s", name);
	strncpy(broadcast_addrs.filename , name, MAXFILENAMESIZE); 
	strncpy(broadcast_addrs.name , name, MAXNAMESIZE); 
	strncpy(broadcast_addrs.local_ip, local->ip, MAXIPV4SIZE);
	strncpy(broadcast_addrs.remote_ip, BROADCAST_IPV4, MAXIPV4SIZE);
	strncpy(addrs.name, name, MAXNAMESIZE);
	broadcast_addrs.local_port = local->port;
	broadcast_addrs.remote_port = BROADCAST_PORT;
	LOGD(__FILE__, __func__, "pull_file() : name = %s", addrs.name);
	strncpy(addrs.local_ip, local->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "pull_file() : local_ip = %s", addrs.local_ip);
	strncpy(addrs.remote_ip, remote->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "pull_file() : remote_ip = %s", addrs.remote_ip);
	addrs.local_port = local->port;
	addrs.remote_port = remote->port;
	LOGD(__FILE__, __func__, "pull_file() : local_port = %hu & remote_port = %hu", addrs.local_port, addrs.remote_port);
	LOGD(__FILE__, __func__, "call start_cntl() with conn = false");
	tryexec(start_cntl(&addrs, &cntl_sock, false));
	CHECK_THREAD(pthread_create(&handle, NULL, thread_start_broadcast, (void *) broadcast_addrs));
	LOGD(__FILE__, __func__, "call accept_cntl()");
	tryexec(accept_cntl(&addrs, &conn_sock, cntl_sock, FOREVER_TIMEOUT));
	CHECK_THREAD(pthread_cancel(&handle));
	LOGD(__FILE__, __func__, "call pull_handshake()");
	tryexec(pull_handshake(path));
	LOGD(__FILE__, __func__, "call start_data()");
	tryexec(start_data(&addrs, &data_sock));
	LOGD(__FILE__, __func__, "call transfer()");
	tryexec(pull_transfer(&seq));
	LOGD(__FILE__, __func__, "call pull_verification()");
	tryexec(pull_verification(&seq));
	LOGT(__FILE__, __func__, "end pull_file() with name %s", addrs.name):
	tryexec(end_file_stream(&filec));
	tryexec(end_data(cast_sock));
	tryexec(end_cntl(cntl_sock));
	tryexec(end_cntl(conn_sock));
	tryexec(end_data(data_sock));
	return _stat;
}

status_t scan_pair(PairInfo *info, size_t *len, addr_pair *local) {
	status_t _stat = SUCCESS;
	CntlAddrs broadcast_addrs = {
		.local_ip = local->ip,
		.local_port = local->port,
		.remote_port = BROADCAST_PORT
	};
	strncpy(broadcast_addrs.remote_ip, BROADCAST_IPV4, MAXIPV4SIZE);
	tryexec(start_scanpair(&broadcast_addrs, &cast_sock, info, len, SCANPAIR_TIMEOUT));
	return _stat;
}

int main(void) {
	return 0;
}
