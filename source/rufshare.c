#include "rufshare.h"

InitConfig *conf;

#ifdef LOGGING
static void *thread_start_logd(void *arg)
{
	status_t *_stat = (status_t *) arg;
	LOGT("in function thread_start_logd()");
	if (start_logd() == -1) {
		*_stat = FAILLOG;
		tryexec(*_stat);
	}
	LOGT("return from thread_start_logd()");
	return arg;
}
#endif

static status_t start_logging(pthread_t *handle)
{
	status_t _stat = SUCCESS;
	LOGT("in function start_logging()");
	#ifdef LOGGING
	CHECK_THREAD(pthread_create(handle, NULL, thread_start_logd, &_stat), "pthread_create() failed to create thread_start_logd thread");
	#else
	handle = NULL;
	#endif
	LOGT("return from start_logging()");
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
	if (conf->tf_trycount <= 0)
		CHECK_STAT(BADCONF, "invalid conf->tf_trycount");
	CHECK_STAT(start_logging(&handle), "start_logging() failed to start logging thread");
	tryexec(start_pusher(path));
	CHECK_THREAD(pthread_cancel(handle), "pthread_cancel() failed to cancel logging thread");
	LOGT("return from push_file()");
	return _stat;
}

status_t pull_file(InitConfig *config, const char *path, char *remote_name)
{
	status_t _stat = SUCCESS;
	conf = config;
	pthread_t handle;
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
	CHECK_STAT(start_logging(&handle), "start_logging() failed to start logging thread");
	tryexec(start_puller(path, remote_name));
	CHECK_THREAD(pthread_cancel(handle), "pthread_cancel() failed to cancel logging thread");
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
	pthread_t handle;
	LOGT("in function scanpair()");
	CHECK_IPV4(conf->addrs.local_ip, "invalid local ip address");
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF, "conf->addrs.local_port = 0");
	if ((conf->spt_cast <= 0) || (conf->sp_interval <= 0) || (conf->sp_trycount))
		CHECK_STAT(BADCONF, "invalid conf->spt_cast or conf->sp_interval or conf->sp_trycount");
	CHECK_STAT(start_logging(&handle), "start_logging() failed to start logging thread");
	tryexec(start_scanpair(info, len));
	CHECK_THREAD(pthread_cancel(handle), "pthread_cancel() failed to cancel logging thread");
	LOGT("return from scanpair()");
	return _stat;
}
