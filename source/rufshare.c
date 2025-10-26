#include "rufshare.h"

RUFShareChunkSize chunk_size;
RUFSharePartialChunkSize partial_chunk_size;
RUFShareChunkCount chunk_count;
sockfd_t cntl_sock;
sockfd_t data_sock;
FileContext filec;

static status_t handshake(const char *path, CntlAddrs *addrs) {
	status_t stat = SUCCESS;
	HeaderArgs header;
	CHECK_STAT(start_file_stream(&filec, path, MRD));
	CHECK_STAT(start_cntl(addrs, *cntl_sock));
	chunk_count = calc_chunk_count(filec.size, chunk_size, &partial_chunk_size);
	SendPacket spacket = pack_RUFShare_SendPacket(chunk_size, chunk_count, partial_chunk_size, 0);
	header.send.packet = spacket;
	CHECK_STAT(push_SEND_header(cntl_sock, &header, HANDSHAKE_SEND_TIMEOUT));
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, HANDSHAKE_RECV_TIMEOUT));
	if (header.recv.packet.ack == 0)
		stat = ZEROACK;
	return stat;
}

static status_t transfer(RUFShareSequence *seq, CntlAddrs *addrs) {
	status_t stat = SUCCESS;
	unsigned short trycount = TRANSFER_TRY_COUNT;
	ChunkContext chcon = {.start_pos = 0, .chunk_size = chunk_size};
	HeaderArgs header0;
	HeaderArgs header1;
	RUFShareCRC32 crc;
	CHECK_EQUAL(0, *seq, ZEROSEQ);
	CHECK_STAT(start_data(addrs, &data_sock));
	while (*seq <= chunk_count) {
		chcon.start_pos = (*seq - 1) * chunk_size;
		chcon.chunk_size = (*seq == chunk_count) ? partial_chunk_size : chunk_size;
		crc = calc_chunk_crc32(&filec, &chcon);
		header.flow = pack_FLOW_FlowPacket(((*seq == chunk_count) ? partial_chunk_size : chunk_size), *seq, crc);
		while (trycount != 0) {
			CHECK_STAT(push_FLOW_header(cntl_sock, &header0, TRANSFER_FLOW_TIMEOUT));
			CHECK_STAT(pull_RECV_header(cntl_sock, &header1, TRANSFER_RECV_TIMEOUT));
			if (header1.recv.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY0);
		trycount = TRANSFER_TRY_COUNT;
		while (trycount != 0) {
			CHECK_STAT(push_chunk_data(data_sock, &filec, &chcon, TRANSFER_DATA_TIMEOUT));
			CHECK_STAT(pull_RECV_header(cntl_sock, &header1, TRANSFER_RECV_TIMEOUT));
			if (header1.recv.ack == 1)
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
	SendPacket spacket = pack_RUFShare_SendPacket(chunk_size, chunk_count, partial_chunk_size, crc);
	header.send.packet = spacket;
	CHECK_STAT(push_SEND_header(cntl_sock, &header, VERIFICATION_SEND_TIMEOUT));
	CHECK_STAT(pull_RECV_header(cntl_sock, &header, VERIFICATION_RECV_TIMEOUT));
	if (header.recv.packet.ack == 0)
		stat = ZEROACK;
	return stat;
}

status_t push_file(const char *name, const char *path, addr_pair *local, addr_pair *remote) {
	status_t stat = SUCCESS;
	RUFShareSequence seq = 1;
	CntlAddrs addrs = {.local_port = local->port, .remote_port = remote->port};
	strncpy(addrs.name, name, MAXNAMESIZE);
	strncpy(addrs.local_ip, local->ip, MAXIPV4SIZE);
	strncpy(addrs.remote_ip, remote->ip, MAXIPV4SIZE);
	extract_file_name(addrs.filename, path, MAXFILENAMESIZE);
	CHECK_STAT(handshake(path, addrs));
	CHECK_STAT(transfer(&seq, addrs));
	CHECK_STAT(verification());
	return stat;
}

status_t pull_file(const char *path, addr_pair *local, addr_pair *remote) {
	status_t stat = SUCCESS;
	RUFShareSequence seq = 1;
	CntlAddrs addrs = {.local_port = local->port, .remote_port = remote->port};
}

status_t scan_pair(PairInfo *info, addr_pair *local);
