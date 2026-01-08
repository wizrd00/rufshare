#include "puller.h"

static void *thread_calc_file_crc16(void *arg)
{
	LOGT("in function thread_calc_file_crc16()");
	RUFShareCRC16 *crc = (RUFShareCRC16 *) arg;
	*crc = calc_file_crc16(&conf->filec);
	LOGD("CRC16 of file calculated with value = %lu", *crc);
	LOGT("return from thread_calc_file_crc16()");
	return arg;
}

static bool match_flow(HeaderArgs *header)
{
	bool val;
	LOGT("in function match_flow()");
	LOGD("check match of conf->seq & conf->chsize");
	if (conf->seq != conf->chcount)
		val = ((header->flow.packet.sequence == conf->seq) && (header->flow.packet.chunk_size == conf->chsize)) ? true : false;
	else
		val = ((header->flow.packet.sequence == conf->seq) && (header->flow.packet.chunk_size == conf->pchsize)) ? true : false;
	LOGT("return from match_flow()");
	return val;
}

static status_t pull_handshake(char *remote_name)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	LOGT("in function pull_handshake()");
	CHECK_STAT(pull_SEND_header(conf->conn_sock, &header, FOREVER_TIMEOUT), "pull_SEND_header() failed");
	LOGD("SEND packet pulled");
	if (!ISVALID_SEND_HEADER(header)) {
		header.recv.packet = pack_RUFShare_RecvPacket(0, 0, 0);	
		LOGD("RECV packet with ack = %d prepared and it is ready to push", header.recv.packet.ack);
		CHECK_STAT(push_RECV_header(conf->conn_sock, &header, conf->hst_recv), "push_RECV_header() failed");
		return _stat;
	}
	conf->chsize = header.send.packet.chunk_size;
	LOGD("conf->chsize = %hu", conf->chsize);
	conf->pchsize = header.send.packet.partial_chunk_size;
	LOGD("conf->pchsize = %hu", conf->pchsize);
	conf->chcount = header.send.packet.chunk_count;
	LOGD("conf->chcount = %u", conf->chcount);
	sstrncpy(conf->filec.name, header.send.info.filename, MAXFILENAMESIZE);
	sstrncpy(conf->addrs.filename, header.send.info.filename, MAXFILENAMESIZE);
	LOGD("conf->addrs.filename = %s", conf->addrs.filename);
	sstrncpy(remote_name, header.send.info.name, MAXNAMESIZE);
	LOGD("remote_name = %s", remote_name);
	sstrncpy(conf->addrs.remote_ip, header.send.info.remote_ip, MAXIPV4SIZE);
	LOGD("conf->addrs.remote_ip = %s", conf->addrs.remote_ip);
	conf->addrs.remote_port = header.send.info.remote_port;
	LOGD("conf->addrs.remote_port = %hu", conf->addrs.remote_port);
	LOGD("required configs have configured");
	conf->filec.size = calc_file_size(conf->chcount, conf->chsize, conf->pchsize);
	LOGD("file size calculated with value = %zu", conf->filec.size);
	header.recv.packet = pack_RUFShare_RecvPacket((start_file_stream(&conf->filec, conf->addrs.filename, MWR) == SUCCESS) ? 1 : 0, 0, 0);
	LOGD("RECV packet with ack = %d prepared and it is ready to push", header.recv.packet.ack);
	CHECK_STAT(push_RECV_header(conf->conn_sock, &header, conf->hst_recv), "push_RECV_header() failed");
	LOGD("RECV packet pushed");
	LOGD("set conf->seq = 1");
	conf->seq = 1;
	LOGT("return from pull_handshake()");
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
	LOGT("in function pull_transfer()");
	while (conf->seq <= conf->chcount) {
		LOGD("prepare chunk with seq = %lu", conf->seq);
		chcon.start_pos = (conf->seq - 1) * conf->chsize;
		chcon.chunk_size = (conf->seq == conf->chcount) ? conf->pchsize : conf->chsize;
		CHECK_STAT(pull_FLOW_header(conf->conn_sock, &flow_header, conf->tft_flow), "pull_FLOW_header() failed");
		LOGD("FLOW packet pulled");
		LOGD("check match of FLOW packet and configs");
		if (match_flow(&flow_header)) {
			LOGD("FLOW packet matched");
			recv_header.recv.packet = pack_RUFShare_RecvPacket(1, 0, conf->seq);
			LOGD("RECV packet with ack = %d prepared and it is ready to push", recv_header.recv.packet.ack);
			CHECK_STAT(push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv), "push_RECV_header() failed");
			LOGD("RECV packet pushed");
			while (trycount != 0) {
				CHECK_STAT(pull_chunk_data(conf->data_sock, &conf->filec, &chcon, conf->tft_data), "pull_chunk_data() failed");
				LOGD("chunk of data pulled");
				crc = calc_chunk_crc32(&conf->filec, &chcon);
				LOGD("crc32 of the chunk calculated with value = %zu", crc);
				if (flow_header.flow.packet.crc == crc) {
					LOGD("crc32 matched");
					recv_header.recv.packet = pack_RUFShare_RecvPacket(1, 0, conf->seq);
					LOGD("RECV packet with ack = %d prepared and it is ready to push", recv_header.recv.packet.ack);
					CHECK_STAT(push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv), "push_RECV_header() failed");
					LOGD("RECV packet pushed");
					break;
				} else {
					LOGD("crc32 did not match");
					recv_header.recv.packet = pack_RUFShare_RecvPacket(0, 0, conf->seq);
					LOGD("RECV packet with ack = %d prepared and it is ready to push", recv_header.recv.packet.ack);
					CHECK_STAT(push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv), "push_RECV_header() failed");
					LOGD("RECV packet pushed");
					LOGD("decrease trycount by one");
					trycount--;
				}
			}
			CHECK_NOTEQUAL(0, trycount, EXPTRY0, "trycount = 0");
			trycount = conf->tf_trycount;
			LOGD("increase conf->seq by one");
			conf->seq++;
		} else {
			LOGD("FLOW packet did not match");
			recv_header.recv.packet = pack_RUFShare_RecvPacket(0, 0, conf->seq);
			LOGD("RECV packet with ack = %d prepared and it is ready to push", recv_header.recv.packet.ack);
			push_RECV_header(conf->conn_sock, &recv_header, conf->tft_recv);
			LOGD("RECV packet pushed");
			_stat = BADFLOW;
			break;
		}
	}
	LOGT("return from pull_transfer()");
	return _stat;
}

static status_t pull_verification(void)
{
	status_t _stat = SUCCESS;
	HeaderArgs header;
	RUFShareCRC16 crc;
	pthread_t handle;
	LOGT("in function pull_verification()");
	CHECK_THREAD(pthread_create(&handle, NULL, thread_calc_file_crc16, (void *) &crc), "pthread_create() failed to create thread_calc_file_crc16 thread");
	LOGD("thread thread_calc_file_crc16() created");
	CHECK_STAT(pull_SEND_header(conf->conn_sock, &header, conf->vft_send), "pull_SEND_header() failed");
	LOGD("SEND packet pulled");
	LOGD("wait for thread_calc_file_crc16() to complete");
	CHECK_THREAD(pthread_join(handle, NULL), "pthread_join() failed");
	if (header.send.packet.crc == crc) {
		header.recv.packet = pack_RUFShare_RecvPacket(1, 0, conf->seq); 
		LOGD("CRC16 matched");
	} else {
		header.recv.packet = pack_RUFShare_RecvPacket(0, 0, conf->seq);
		LOGD("CRC16 didn't match");
		_stat = FAILCRC;
	}
	LOGD("RECV prepared and it is ready to push");
	CHECK_STAT(push_RECV_header(conf->conn_sock, &header, conf->vft_recv), "push_RECV_header() failed");
	LOGD("RECV packet pushed");
	LOGT("return from pull_verification()");
	return _stat;
}

status_t start_puller(char *remote_name)
{
	status_t _stat = SUCCESS;
	LOGT("in function start_puller()");
	CHECK_STAT(start_cntl(&conf->addrs, &conf->cntl_sock, false), "start_cntl() failed");
	CHECK_STAT(accept_cntl(&conf->addrs, &conf->conn_sock, conf->cntl_sock, FOREVER_TIMEOUT), "accept_cntl() failed");
	CHECK_STAT(close_socket(conf->cast_sock), "close_socket() failed on socket with fd = %d", conf->cast_sock);
	CHECK_STAT(pull_handshake(remote_name), "pull_handshake() failed");
	CHECK_STAT(start_data(&conf->addrs, &conf->data_sock), "start_data() failed");
	conf->segsize = calc_segment_size((conf->chsize != 0) ? (size_t) conf->chsize : (size_t) conf->pchsize);
	LOGD("segment size calculated with value = %zu", conf->segsize);
	CHECK_STAT(pull_transfer(), "pull_transfer() failed");
	CHECK_STAT(pull_verification(), "pull_verification() failed");
	CHECK_STAT(end_file_stream(&conf->filec), "end_file_stream() failed");
	CHECK_STAT(end_cntl(conf->cntl_sock), "end_cntl() failed");
	CHECK_STAT(end_data(conf->data_sock), "end_data() failed");
	CHECK_STAT(end_cntl(conf->conn_sock), "end_cntl() failed");
	LOGT("return from start_puller()");
	return _stat;
}
