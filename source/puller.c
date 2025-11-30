#include "puller.h"

static void *thread_calc_file_crc16(void *arg)
{
	RUFShareCRC16 *crc = (RUFShareCRC16 *) arg;
	*crc = calc_file_crc16(&conf->filec);
	return arg;
}

static bool match_flow(HeaderArgs *header)
{
	return ((header->flow.packet.sequence != conf->seq) || (header->flow.packet.chunk_size != conf->chsize)) ? false : true;
}

static status_t pull_handshake(const char *path, char *remote_name)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	CHECK_STAT(pull_SEND_header(conf->conn_sock, &header, FOREVER_TIMEOUT));
	if (ISVALID_SEND_HEADER(header)) {
		header.recv.packet = pack_RUFShare_RecvPacket(0, 0, 0);	
		CHECK_STAT(push_RECV_header(conf->conn_sock, &header, conf->hst_recv));
		return _stat;
	}
	conf->chsize = header.send.packet.chunk_size;
	conf->pchsize = header.send.packet.partial_chunk_size;
	conf->chcount = header.send.packet.chunk_count;
	sstrncpy(conf->filec.name, header.send.info.filename, MAXFILENAMESIZE);
	sstrncpy(conf->addrs.filename, header.send.info.filename, MAXFILENAMESIZE);
	sstrncpy(remote_name, header.send.info.name, MAXNAMESIZE);
	sstrncpy(conf->addrs.remote_ip, header.send.info.local_ip, MAXIPV4SIZE);
	conf->addrs.remote_port = header.send.info.local_port;
	conf->filec.size = calc_file_size(conf->chcount, conf->chsize, conf->pchsize);
	header.recv.packet = pack_RUFShare_RecvPacket((start_file_stream(&conf->filec, path, MWR) == SUCCESS) ? 1 : 0, 0, 0);
	CHECK_STAT(push_RECV_header(conf->conn_sock, &header, conf->hst_recv));
	conf->seq = 1;
	return _stat;
}

static status_t pull_transfer(void)
{
	status_t _stat = SUCCESS;
	ChunkContext chcon;
	HeaderArgs flow_header;
	HeaderArgs recv_header;
	RUFShareCRC32 crc;
	int trycount = conf->tf_trycount;
	while (conf->seq <= conf->chcount) {
		chcon.start_pos = (conf->seq - 1) * conf->chsize;
		chcon.chunk_size = (conf->seq == conf->chcount) ? conf->pchsize : conf->chsize;
		CHECK_STAT(pull_FLOW_header(conf->conn_sock, &flow_header, conf->tft_flow));
		if (match_flow(&flow_header)) {
			recv_header.recv.packet = pack_RUFShare_RecvPacket(1, 0, conf->seq);
			CHECK_STAT(push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv));
			while (trycount != 0) {
				CHECK_STAT(pull_chunk_data(conf->data_sock, &conf->filec, &chcon, conf->tft_data));
				crc = calc_chunk_crc32(&conf->filec, &chcon);
				if (flow_header.flow.packet.crc == crc) {
					recv_header.recv.packet = pack_RUFShare_RecvPacket(1, 0, conf->seq);
					CHECK_STAT(push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv));
					break;
				}
				else {
					recv_header.recv.packet = pack_RUFShare_RecvPacket(0, 0, conf->seq);
					CHECK_STAT(push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv));
					trycount--;
				}
			}
			CHECK_NOTEQUAL(0, trycount, EXPTRY0);
			trycount = conf->tf_trycount;
			conf->seq++;
		}
		else {
			recv_header.recv.packet = pack_RUFShare_RecvPacket(0, 0, conf->seq);
			push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv);
			_stat = BADFLOW;
			break;
		}
	}
	conf->seq = 0;
	return _stat;
}

static status_t pull_verification(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	RUFShareCRC16 crc;
	pthread_t handle;
	CHECK_THREAD(pthread_create(&handle, NULL, thread_calc_file_crc16, (void *) &crc));
	CHECK_STAT(pull_SEND_header(conf->conn_sock, &header, conf->vft_send));
	CHECK_THREAD(pthread_join(handle, NULL));
	header.recv.packet = pack_RUFShare_RecvPacket((header.send.packet.crc == crc) ? 1 : 0, 0, conf->seq); 
	CHECK_STAT(push_RECV_header(conf->conn_sock, &header, conf->vft_recv));
	if (header.send.packet.crc != crc) {
		_stat = FAILCRC;
	}
	return _stat;
}

status_t start_puller(const char *path, char *remote_name)
{
	status_t _stat = SUCCESS;
	CHECK_STAT(start_cntl(&conf->addrs, &conf->cntl_sock, false));
	CHECK_STAT(accept_cntl(&conf->addrs, &conf->conn_sock, conf->cntl_sock, FOREVER_TIMEOUT));
	CHECK_STAT(pull_handshake(path, remote_name));
	CHECK_STAT(start_data(&conf->addrs, &conf->data_sock));
	conf->segsize = calc_segment_size((conf->chsize != 0) ? (size_t) conf->chsize : (size_t) conf->pchsize);
	CHECK_STAT(pull_transfer());
	CHECK_STAT(pull_verification());
	CHECK_STAT(end_file_stream(&conf->filec));
	CHECK_STAT(end_cntl(conf->cntl_sock));
	CHECK_STAT(end_data(conf->data_sock));
	CHECK_STAT(end_cntl(conf->conn_sock));
	return _stat;
}
