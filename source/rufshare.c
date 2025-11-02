#include "rufshare.h"

RUFShareChunkSize chunk_size;
RUFSharePartialChunkSize partial_chunk_size;
RUFShareChunkCount chunk_count;
sockfd_t cntl_sock;
sockfd_t data_sock;
sockfd_t conn_sock;
FileContext filec;

static void set_global_variables(HeaderArgs *header) {
	chunk_size = header->send.packet.chunk_size;
	partial_chunk_size = header->send.packet.partial_chunk_size;
	chunk_count = header->send.packet.chunk_count;
	return;
}

static status_t handshake(void) {
	status_t stat = SUCCESS;
	HeaderArgs header;
	LOGT(__FILE__, __func__, "start handshake");
	chunk_count = calc_chunk_count(filec.size, chunk_size, &partial_chunk_size);
	LOGD(__FILE__, __func__, "file with size %lu, splitted into %lu chunks with size %lu", filec.size, chunk_count, chunk_size);
	header.send.packet = pack_RUFShare_SendPacket(chunk_size, chunk_count, partial_chunk_size, 0);
	CHECK_STAT(push_SEND_header(cntl_sock, &header, HANDSHAKE_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pushed on socked fd %d with %d timeout", cntl_sock, HANDSHAKE_SEND_TIMEOUT);
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, HANDSHAKE_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pulled on socked fd %d with %d timeout", cntl_sock, HANDSHAKE_RECV_TIMEOUT);
	if (header.recv.packet.ack == 0)
		stat = ZEROACK;
	LOGD(__FILE__, __func__, "header.recv.packet.ack = %d", header.recv.packet.ack);
	return stat;
}

static status_t transfer(RUFShareSequence *seq) {
	status_t stat = SUCCESS;
	unsigned short trycount = TRANSFER_TRY_COUNT;
	ChunkContext chcon;
	HeaderArgs header0;
	HeaderArgs header1;
	RUFShareCRC32 crc;
	CHECK_EQUAL(0, *seq, ZEROSEQ);
	while (*seq <= chunk_count) {
		chcon.start_pos = (*seq - 1) * chunk_size;
		chcon.chunk_size = (*seq == chunk_count) ? partial_chunk_size : chunk_size;
		crc = calc_chunk_crc32(&filec, &chcon);
		header.flow.packet = pack_FLOW_FlowPacket(((*seq == chunk_count) ? partial_chunk_size : chunk_size), *seq, crc);
		while (trycount != 0) {
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
	return stat;
}

static status_t verification(void) {
	status_t stat = SUCCESS;
	HeaderArgs header;
	RUFShareCRC16 crc = calc_file_crc16(&filec);
	header.send.packet = pack_RUFShare_SendPacket(chunk_size, chunk_count, partial_chunk_size, crc);
	CHECK_STAT(push_SEND_header(cntl_sock, &header, VERIFICATION_SEND_TIMEOUT));
	LOGD(__FILE__, __func__, "SEND packet pushed on socket fd %d with timeout %d", cntl_sock, VERIFICATION_SEND_TIMEOUT);
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, VERIFICATION_RECV_TIMEOUT));
	LOGD(__FILE__, __func__, "RECV packet pulled on socked fd %d with %d timeout", cntl_sock, VERIFICATION_RECV_TIMEOUT);
	if (header.recv.packet.ack == 0)
		stat = ZEROACK;
	LOGD(__FILE__, __func__, "header.recv.packet.ack = %d", header.recv.packet.ack);
	return stat;
}

status_t push_file(const char *name, const char *path, addr_pair *local, addr_pair *remote) {
	status_t stat = SUCCESS;
	RUFShareSequence seq = 1;
	CntlAddrs cntl_addrs = {.local_port = local->port, .remote_port = remote->port};
	LOGT(__FILE__, __func__, "start push_file with name %s", name);
	strncpy(addrs.name, name, MAXNAMESIZE);
	LOGD(__FILE__, __func__, "push_file() : name = %s", addrs.name);
	strncpy(addrs.local_ip, local->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "push_file() : local_ip = %s", addrs.local_ip);
	strncpy(addrs.remote_ip, remote->ip, MAXIPV4SIZE);
	LOGD(__FILE__, __func__, "push_file() : remote_ip = %s", addrs.remote_ip);
	extract_file_name(addrs.filename, path, MAXFILENAMESIZE);
	LOGD(__FILE__, __func__, "push_file() : filename = %s", addrs.filename);
	tryexec_start_file_stream(start_file_stream(&filec, path, MRD));
	LOGD(__FILE__, __func__, "call start_cntl()");
	tryexec_start_cntl(start_cntl(&addrs, &cntl_sock, true));
	LOGD(__FILE__, __func__, "call handshake()");
	tryexec_handshake(handshake());
	LOGD(__FILE__, __func__, "call start_data()");
	tryexec_start_data(start_data(&addrs, &data_sock));
	LOGD(__FILE__, __func__, "call transfer()");
	tryexec_transfer(transfer(&seq));
	LOGD(__FILE__, __func__, "call verification()");
	tryexec_verification(verification());
	LOGD(__FILE__, __func__, "end push_file with name %s", addrs.name):
	tryexec_end_file_stream(end_file_stream(&filec));
	tryexec_end_cntl(end_cntl(cntl_sock));
	tryexec_end_data(end_data(data_sock));
	return stat;
}

status_t pull_file(addr_pair *local, addr_pair *remote) {
	status_t stat = SUCCESS;
	BroadCast bc_handle;
	RUFShareSequence seq = 0;
	CntlAddrs addrs = {.local_port = local->port, .remote_port = remote->port};
	HeaderArgs storage_header;
	HeaderArgs header0;
	HeaderArgs header1;
	ChunkContext chcon;
	RUFShareCRC16 file_crc;
	RUFShareCRC32 chunk_crc;
	char *fname = storage_header.send.info.filename;
	strncpy(addrs.local_ip, local->ip, MAXIPV4SIZE);
	strncpy(addrs.remote_ip, remote->ip, MAXIPV4SIZE);
	CHECK_STAT(start_broadcast(&bc_handle));
	CHECK_STAT(start_cntl(&addrs, cntl_sock, false)); 
	CHECK_STAT(start_data(&addrs, data_sock));
	CHECK_STAT(accept_cntl(&addrs, &conn_sock, cntl_sock));
	CHECK_STAT(pull_SEND_header(conn_sock, &storage_header, HANDSHAKE_SEND_TIMEOUT));
	set_global_variables(&storage_header);
	CHECK_STAT(start_file_stream(&filec, ((fname[0] == '\0') ? make_random_file(fname, MAXFILENAMESIZE, "RecvFile") : fname), MWR));
	// TODO : what start_file_stream returns, specifies ack of RecvPacket
	header1.recv.packet = pack_RUFShare_RecvPacket(1, 0, seq);
	CHECK_STAT(push_RECV_header(conn_sock, &header1, HANDSHAKE_RECV_TIMEOUT));
	while (seq <= chunk_count) {
		while (trycount != 0) {
			CHECK_STAT(pull_FLOW_header(conn_sock, &header0, TRANSFER_FLOW_TIMEOUT));
			chunk_crc = header0.flow.packet.crc;
			if () {
				// TODO : some matchings that specify ack of RecvPacket and trycount--
				header1.recv.packet = pack_RUFShare_RecvPacket(1, 0, seq);
				CHECK_STAT(push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
				break;
			}
			else {
				header1.recv.packet = pack_RUFShare_RecvPacket(0, 0, seq);
				CHECK_STAT(push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
				trycount--;
			}
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY1);
		trycount = TRANSFER_TRY_COUNT;
		while (trycount != 0) {
			chcon.start_pos = (seq - 1) * chunk_size;
			chcon.chunk_size = (seq == chunk_count) ? partial_chunk_size : chunk_size; 
			CHECK_STAT(pull_chunk_data(data_sock, &filec, &chcon, TRANSFER_DATA_TIMEOUT));
			if (calc_chunk_crc32(&filec, &chcon) == chunk_crc) {
				header1.recv.packet = pack_RUFShare_RecvPacket(1, 0, seq);
				CHECK_STAT(push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
				break;
			}
			else {
				header1.recv.packet = pack_RUFShare_RecvPacket(0, 0, seq);
				CHECK_STAT(push_RECV_header(conn_sock, &header1, TRANSFER_RECV_TIMEOUT));
				trycount--;
			}
		}
		CHECK_STAT(pull_SEND_header(conn_sock, &header0, VERIFICATION_SEND_TIMEOUT));
		file_crc = calc_file_crc16(&filec);
		if (file_crc == header0.send.packet.crc) {
			header1.recv.packet = pack_RUFShare_RecvPacket(1, file_crc, seq);
			CHECK_STAT(push_RECV_header(conn_sock, &header1, VERIFICATION_RECV_TIMEOUT));
		}
		else {
			header1.recv.packet = pack_RUFShare_RecvPacket(0, file_crc, seq);
			CHECK_STAT(push_RECV_header(conn_sock, &header1, VERIFICATION_RECV_TIMEOUT));
		}
	}
	end_file_stream(&filec);
	end_cntl(cntl_sock);
	end_data(data_sock);
	end_cntl(conn_sock);
	return;
	// TODO : add error exceptions and a terminate function to close sockets instead of CHECK_STAT
}

status_t scan_pair(PairInfo *info, addr_pair *local);
