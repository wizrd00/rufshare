#include "pusher.h"

static status_t push_handshake(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	conf->chcount = calc_chunk_count(conf->filec.size, conf->chsize, &conf->pchsize);
	header.send.packet = pack_RUFShare_SendPacket(conf->chsize, conf->chcount, conf->pchsize, 0);
	sstrncpy(header.send.info.filename, conf->addrs.filename, MAXFILENAMESIZE);
	sstrncpy(header.send.info.name, conf->addrs.name, MAXNAMESIZE);
	sstrncpy(header.send.info.local_ip, conf->addrs.local_ip, MAXIPV4SIZE);
	sstrncpy(header.send.info.remote_ip, conf->addrs.remote_ip, MAXIPV4SIZE);
	header.send.info.local_port = conf->addrs.local_port;
	header.send.info.remote_port = conf->addrs.remote_port;
	CHECK_STAT(push_SEND_header(conf->cntl_sock, &header, conf->hst_send));
	CHECK_STAT(pull_RECV_header(conf->cntl_sock, &header, conf->hst_recv));
	if (header.recv.packet.ack == 0)
		_stat = ZEROACK;
	conf->seq = 1;
	return _stat;
}

static status_t push_transfer(void)
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
		crc = calc_chunk_crc32(&conf->filec, &chcon);
		flow_header.flow.packet = pack_RUFShare_FlowPacket(chcon.chunk_size, conf->seq, crc);
		while (trycount != 0) {
			CHECK_STAT(push_FLOW_header(conf->cntl_sock, &flow_header, conf->tft_flow));
			CHECK_STAT(pull_RECV_header(conf->cntl_sock, &recv_header, conf->tft_recv));
			if (recv_header.recv.packet.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY0);
		trycount = conf->tf_trycount;
		while (trycount != 0) {
			CHECK_STAT(push_chunk_data(conf->data_sock, &conf->filec, &chcon, conf->tft_data));
			CHECK_STAT(pull_RECV_header(conf->cntl_sock, &recv_header, conf->tft_recv));
			if (recv_header.recv.packet.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY1);
		trycount = conf->tf_trycount;
		conf->seq++;
	}
	conf->seq = 0;
	return _stat;
}

static status_t push_verification(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	RUFShareCRC16 crc = calc_file_crc16(&conf->filec);
	header.send.packet = pack_RUFShare_SendPacket(conf->chsize, conf->chcount, conf->pchsize, crc);
	sstrncpy(header.send.info.filename, conf->addrs.filename, MAXFILENAMESIZE);
	sstrncpy(header.send.info.name, conf->addrs.name, MAXNAMESIZE);
	sstrncpy(header.send.info.local_ip, conf->addrs.local_ip, MAXIPV4SIZE);
	sstrncpy(header.send.info.remote_ip, conf->addrs.remote_ip, MAXIPV4SIZE);
	header.send.info.local_port = conf->addrs.local_port;
	header.send.info.remote_port = conf->addrs.remote_port;
	CHECK_STAT(push_SEND_header(conf->cntl_sock, &header, conf->vft_send));
	CHECK_STAT(pull_RECV_header(conf->cntl_sock, &header, conf->vft_recv));
	if (header.recv.packet.ack == 0)
		_stat = ZEROACK;
	return _stat;
}

status_t start_pusher(const char *path)
{
	status_t _stat = SUCCESS;
	CHECK_STAT(start_file_stream(&conf->filec, path, MRD));
	CHECK_STAT(start_cntl(&conf->addrs, &conf->cntl_sock, true));
	CHECK_STAT(push_handshake());
	CHECK_STAT(start_data(&conf->addrs, &conf->data_sock));
	CHECK_STAT(push_transfer());
	CHECK_STAT(push_verification());
	CHECK_STAT(end_file_stream(&conf->filec));
	CHECK_STAT(end_cntl(conf->cntl_sock));
	CHECK_STAT(end_data(conf->data_sock));
	return _stat;
}
