#include "rufshare.h"

InitConfig *conf;

static void *thread_broadcast(void *arg)
{
	status_t _stat = SUCCESS;
	InitConfig *config = (InitConfig *) arg;
	LOGT("in function thread_broadcast()");
	if (broadcast(config) != SUCCESS)
		return NULL;
	LOGT("return from thread_broadcast()");
	return config;
}

status_t initiate(const char *logpath)
{
	status_t _stat = SUCCESS;
	#ifdef LOGGING
	CHECK_INT(init_logd(logpath), FAILLOG, "init_logd() failed");
	#endif
	return _stat;
}

status_t push_file(InitConfig *config, const char *path)
{
	status_t _stat = SUCCESS;
	conf = config;
	pthread_t handle;
	char *filename;
	LOGT("in function push_file()");
	CHECK_NOTEQUAL(0, conf->chsize, BADCONF, "conf->chsize = 0");
	CHECK_EQUAL(0, conf->pchsize, BADCONF, "conf->pchsize = 0");
	CHECK_EQUAL(0, conf->chcount, BADCONF, "conf->chcount = 0");
	CHECK_EQUAL(0, conf->seq, BADCONF, "cond->seq = 0");
	CHECK_NOTEQUAL(0, conf->addrs.filename[0], BADCONF, "no string in conf->addrs.filename");
	CHECK_NOTEQUAL(0, conf->addrs.name[0], BADCONF, "no string in conf->addrs.name");
	CHECK_IPV4(conf->addrs.local_ip, "invalid local ip address");
	CHECK_IPV4(conf->addrs.remote_ip, "invalid remote ip address");
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF, "conf->addrs.local_port = 0");
	CHECK_NOTEQUAL(0, conf->addrs.remote_port, BADCONF, "conf->addrs.remote_port = 0");
	if ((conf->tf_trycount <= 0) || (conf->hst_send <= 0) || (conf->hst_recv <= 0) || (conf->tft_flow <= 0) || (conf->tft_recv <= 0) || (conf->tft_data <= 0) || (conf->vft_send <= 0) || (conf->vft_recv <= 0))
		CHECK_STAT(BADCONF, "invalid conf->tf_trycount");
	tryexec(start_pusher(path));
	CHECK_THREAD(pthread_cancel(handle), "pthread_cancel() failed to cancel logging thread");
	LOGT("return from push_file()");
	return _stat;
}

status_t pull_file(InitConfig *config, char *remote_name)
{
	status_t _stat = SUCCESS;
	conf = config;
	pthread_t bc_handle;
	LOGT("in function pull_file()");
	CHECK_EQUAL(0, conf->chsize, BADCONF, "conf->chsize = 0");
	CHECK_EQUAL(0, conf->pchsize, BADCONF, "conf->pchsize = 0");
	CHECK_EQUAL(0, conf->chcount, BADCONF, "conf->chcount = 0");
	CHECK_EQUAL(0, conf->seq, BADCONF, "conf->seq = 0");
	CHECK_NOTEQUAL(0, conf->addrs.name[0], BADCONF, "no string in conf->addrs.name");
	CHECK_IPV4(conf->addrs.local_ip, "invalid local ip address");
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF, "conf->addrs.local_port = 0");
	if (conf->tf_trycount <= 0)
		CHECK_STAT(BADCONF, "invalid conf->tf_trycount");
	CHECK_THREAD(pthread_create(&bc_handle, NULL, thread_broadcast, (void *) config), "pthread_create() failed to create thread_broadcast thread");
	tryexec(start_puller(remote_name));
	CHECK_THREAD(pthread_cancel(bc_handle), "pthread_cancel() failed to cancel broadcast thread");
	LOGT("return from pull_file()");
	return _stat;
}

status_t broadcast(InitConfig *config)
{
	status_t _stat = SUCCESS;
	conf = config;
	CntlAddrs addrs;
	LOGT("in function broadcast()");
	CHECK_NOTEQUAL(0, conf->addrs.name[0], BADCONF, "no string in conf->addrs.name");
	CHECK_IPV4(conf->addrs.local_ip, "invalid local ip address");
	CHECK_IPV4(conf->addrs.remote_ip, "invalid remote ip address");
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF, "conf->addrs.local_port = 0");
	CHECK_NOTEQUAL(0, conf->addrs.remote_port, BADCONF, "conf->addrs.remote_port = 0");
	if ((conf->bct_cast <= 0) || (conf->bc_interval <= 0) || (conf->bc_trycount <= 0))
		CHECK_STAT(BADCONF, "invalid conf->bct_cast or conf->bc_interval or conf->bc->trycount");
	tryexec(start_broadcast());
	LOGT("return from broadcast()");
	return _stat;
}

status_t scanpair(InitConfig *config, PairInfo *info, size_t *len)
{
	status_t _stat = SUCCESS;
	conf = config;
	LOGT("in function scanpair()");
	CHECK_IPV4(conf->addrs.local_ip, "invalid local ip address");
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF, "conf->addrs.local_port = 0");
	if ((conf->spt_cast <= 0) || (conf->sp_interval <= 0) || (conf->sp_trycount <= 0))
		CHECK_STAT(BADCONF, "invalid conf->spt_cast or conf->sp_interval or conf->sp_trycount");
	tryexec(start_scanpair(info, len));
	LOGT("return from scanpair()");
	return _stat;
}
