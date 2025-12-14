#include "pusher.h"

static status_t push_handshake(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	LOGT("in function push_handshake()");
	conf->chcount = calc_chunk_count(conf->filec.size, conf->chsize, &conf->pchsize);
	header.send.packet = pack_RUFShare_SendPacket(conf->chsize, conf->chcount, conf->pchsize, 0);
	sstrncpy(header.send.info.filename, conf->addrs.filename, MAXFILENAMESIZE);
	sstrncpy(header.send.info.name, conf->addrs.name, MAXNAMESIZE);
	sstrncpy(header.send.info.local_ip, conf->addrs.local_ip, MAXIPV4SIZE);
	sstrncpy(header.send.info.remote_ip, conf->addrs.remote_ip, MAXIPV4SIZE);
	header.send.info.local_port = conf->addrs.local_port;
	header.send.info.remote_port = conf->addrs.remote_port;
	CHECK_STAT(push_SEND_header(conf->cntl_sock, &header, conf->hst_send), "push_SEND_header() failed");
	CHECK_STAT(pull_RECV_header(conf->cntl_sock, &header, conf->hst_recv), "pull_RECV_header() failed");
	if (header.recv.packet.ack == 0)
		_stat = ZEROACK;
	conf->seq = 1;
	LOGT("return from push_handshake()");
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
	LOGT("in function push_transfer()");
	while (conf->seq <= conf->chcount) {
		chcon.start_pos = (conf->seq - 1) * conf->chsize;
		chcon.chunk_size = (conf->seq == conf->chcount) ? conf->pchsize : conf->chsize;
		crc = calc_chunk_crc32(&conf->filec, &chcon);
		flow_header.flow.packet = pack_RUFShare_FlowPacket(chcon.chunk_size, conf->seq, crc);
		while (trycount != 0) {
			CHECK_STAT(push_FLOW_header(conf->cntl_sock, &flow_header, conf->tft_flow), "push_FLOW_header() failed");
			CHECK_STAT(pull_RECV_header(conf->cntl_sock, &recv_header, conf->tft_recv), "pull_RECV_header() failed");
			if (recv_header.recv.packet.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY0, "first trycount = 0");
		trycount = conf->tf_trycount;
		while (trycount != 0) {
			CHECK_STAT(push_chunk_data(conf->data_sock, &conf->filec, &chcon, conf->tft_data), "push_chunk_data() failed");
			CHECK_STAT(pull_RECV_header(conf->cntl_sock, &recv_header, conf->tft_recv), "pull_RECV_header() failed");
			if (recv_header.recv.packet.ack == 1)
				break;
			else
				trycount--;
		}
		CHECK_NOTEQUAL(0, trycount, EXPTRY1, "second trycount = 0");
		trycount = conf->tf_trycount;
		conf->seq++;
	}
	conf->seq = 0;
	LOGT("return from push_transfer()");
	return _stat;
}

static status_t push_verification(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	LOGT("in function push_verification()");
	RUFShareCRC16 crc = calc_file_crc16(&conf->filec);
	header.send.packet = pack_RUFShare_SendPacket(conf->chsize, conf->chcount, conf->pchsize, crc);
	sstrncpy(header.send.info.filename, conf->addrs.filename, MAXFILENAMESIZE);
	sstrncpy(header.send.info.name, conf->addrs.name, MAXNAMESIZE);
	sstrncpy(header.send.info.local_ip, conf->addrs.local_ip, MAXIPV4SIZE);
	sstrncpy(header.send.info.remote_ip, conf->addrs.remote_ip, MAXIPV4SIZE);
	header.send.info.local_port = conf->addrs.local_port;
	header.send.info.remote_port = conf->addrs.remote_port;
	CHECK_STAT(push_SEND_header(conf->cntl_sock, &header, conf->vft_send), "push_SEND_header() failed");
	CHECK_STAT(pull_RECV_header(conf->cntl_sock, &header, conf->vft_recv), "pull_RECV_header() failed");
	if (header.recv.packet.ack == 0)
		_stat = ZEROACK;
	LOGT("return from push_verification()");
	return _stat;
}

status_t start_pusher(const char *path)
{
	status_t _stat = SUCCESS;
	LOGT("in function start_pusher()");
	CHECK_STAT(start_file_stream(&conf->filec, path, MRD), "start_file_stream() with mode MRD failed");
	CHECK_STAT(start_cntl(&conf->addrs, &conf->cntl_sock, true), "start_cntl() failed");
	CHECK_STAT(push_handshake(), "push_handshake() failed");
	CHECK_STAT(start_data(&conf->addrs, &conf->data_sock), "start_data() failed");
	CHECK_STAT(push_transfer(), "push_transfer() failed");
	CHECK_STAT(push_verification(), "push_verification() failed");
	CHECK_STAT(end_file_stream(&conf->filec), "end_file_stream() failed");
	CHECK_STAT(end_cntl(conf->cntl_sock), "end_cntl() failed");
	CHECK_STAT(end_data(conf->data_sock), "end_data() failed");
	LOGT("return from start_pusher()");
	return _stat;
}
