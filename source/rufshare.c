#include "rufshare.h"

InitConfig *conf;

#ifdef LOGGING
static void *thread_start_logd(void *arg) {
	status_t *_stat = (status_t *) arg;
	if (start_logd() == -1) {
		*_stat = FAILLOG;
		tryexec(*_stat);
	}
	return arg;
}
#endif

static status_t start_logging(pthread_t *handle) {
	status_t _stat = SUCCESS;
	#ifdef LOGGING
	CHECK_THREAD(pthread_create(handle, NULL, thread_start_logd, &_stat));
	#else
	handle = NULL;
	#endif
	return _stat;
}

status_t push_file(const char *path) {
	status_t _stat = SUCCESS;
	pthread_t handle;
	char *filename;
	CHECK_NOTEQUAL(0, conf->chsize, BADCONF);
	CHECK_EQUAL(0, conf->pchsize, BADCONF);
	CHECK_EQUAL(0, conf->chcount, BADCONF);
	CHECK_EQUAL(0, conf->seq, BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.filename[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.name[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.local_ip[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.remote_ip[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.remote_port, BADCONF);
	if (conf->segsize > conf->chsize)
		return _stat = BADCONF;
	if (conf->tf_trycount <= 0)
		return _stat = BADCONF;
	CHECK_STAT(start_logging(&handle));
	tryexec(start_pusher(path));
	CHECK_THREAD(pthread_cancel(handle));
	return _stat;
}

status_t pull_file(const char *path, char *remote_name) {
	status_t _stat = SUCCESS;
	pthread_t handle;
	CHECK_EQUAL(0, conf->chsize, BADCONF);
	CHECK_EQUAL(0, conf->pchsize, BADCONF);
	CHECK_EQUAL(0, conf->chcount, BADCONF);
	CHECK_EQUAL(0, conf->seq, BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.name[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.local_ip[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF);
	if (conf->tf_trycount <= 0)
		return _stat = BADCONF;
	CHECK_STAT(start_logging(&handle));
	tryexec(start_puller(path, remote_name));
	CHECK_THREAD(pthread_cancel(handle));
	return _stat;
}

status_t broadcast(void) {
	status_t _stat = SUCCESS;
	CntlAddrs addrs;
	CHECK_NOTEQUAL(0, conf->addrs.name, BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.local_ip[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.remote_ip[0], BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.remote_port, BADCONF);
	if ((conf->bct_cast <= 0) || (conf->bc_interval <= 0) || (conf->bc_trycount <= 0))
		return _stat = BADCONF;
	tryexec(start_broadcast());
	return _stat;
}

status_t scanpair(PairInfo *info, size_t *len) {
	status_t _stat = SUCCESS;
	pthread_t handle;
	CHECK_NOTEQUAL(0, conf->addrs.local_ip, BADCONF);
	CHECK_NOTEQUAL(0, conf->addrs.local_port, BADCONF);
	if ((conf->spt_cast <= 0) || (conf->sp_interval <= 0) || (conf->sp_trycount))
		return _stat = BADCONF;
	tryexec(start_scanpair(info, len));
	CHECK_STAT(start_logging(&handle));
	CHECK_THREAD(pthread_cancel(handle));
	return _stat;
}
