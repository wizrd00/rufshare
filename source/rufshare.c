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
	LOGT(__FILE__, __func__, "start transfer");
	CHECK_EQUAL(0, *seq, ZEROSEQ);
	while (*seq <= chunk_count) {
		chcon.start_pos = (*seq - 1) * chunk_size;
		chcon.chunk_size = (*seq == chunk_count) ? partial_chunk_size : chunk_size;
		crc = calc_chunk_crc32(&filec, &chcon);
		header.flow.packet = pack_FLOW_FlowPacket(((*seq == chunk_count) ? partial_chunk_size : chunk_size), *seq, crc);
		LOGD(__FILE__, __func__, "start_pos = %lu, chunk_size = %lu, crc = %lu", chcon.start_pos, chcon.chunk_size, crc);
		while (trycount != 0) {
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
			LOGD(__FILE__, __func__, "sendign chunk data with size %lu", chcon.chunk_size);
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
	LOGD(__FILE__, __func__, "transfer complete");
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
	LOGD(__FILE__, __func__, "end push_file() with name %s", addrs.name):
	tryexec_end_file_stream(end_file_stream(&filec));
	tryexec_end_cntl(end_cntl(cntl_sock));
	tryexec_end_data(end_data(data_sock));
	return stat;
}

status_t pull_file(addr_pair *local, addr_pair *remote) {
	status_t stat = SUCCESS;
	return stat;
}

status_t scan_pair(PairInfo *info, addr_pair *local);
