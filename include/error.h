#ifndef RUFSHARE_ERROR_H
#define RUFSHARE_ERROR_H

#include "types.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define GLOBAL_ERROR_TEXT(stat) "\n[\x1b[1;31mERROR\x1b[0m] -> " #stat "\n\tstat -> %s" "\n\terrno -> %s (maybe not related)" "\n\tfunc -> %s()\n"
#define FAILURE_ERROR_TEXT "process failed"
#define TIMEOUT_ERROR_TEXT "process timed out"
#define LOWSIZE_ERROR_TEXT "process received size that is lower than expected" 
#define EXPTRY0_ERROR_TEXT "try count reached zero"
#define EXPTRY1_ERROR_TEXT "try count reached zero"
#define ZEROACK_ERROR_TEXT "process received RECV-NACK"
#define ZEROSEQ_ERROR_TEXT "transfer function received sequence = 0"
#define FAILSET_ERROR_TEXT "process failed to set socket option"
#define FAILCRC_ERROR_TEXT "file crc does not match"
#define FAILLOG_ERROR_TEXT "init_logd() failed"
#define BADCONF_ERROR_TEXT "bad config to initiate the process"
#define BADFLOW_ERROR_TEXT "process received bad flow packet"
#define BADARGS_ERROR_TEXT "process received bad argument"
#define BADINET_ERROR_TEXT "process received invalid address family"
#define BADTYPE_ERROR_TEXT "process received invalid header type"
#define BADIPV4_ERROR_TEXT "process received invalid ipv4 address format"
#define BADPORT_ERROR_TEXT "process received invalid port number"
#define NOMFILE_ERROR_TEXT "invalid mfile"
#define NOFSTAT_ERROR_TEXT "fstat() function failed"
#define NOAVAIL_ERROR_TEXT "there is no enough space on disk"
#define NOCREAT_ERROR_TEXT "process can not create file"
#define NOTRUNC_ERROR_TEXT "ftruncate() function failed"
#define NOCLOSE_ERROR_TEXT "fclose() function failed"
#define EMFSEEK_ERROR_TEXT "mfseek() function failed"
#define EMFSYNC_ERROR_TEXT "mfsync() function failed"
#define ERRBIND_ERROR_TEXT "bind() function failed"
#define ERRCONN_ERROR_TEXT "connect() function failed"
#define ERRRECV_ERROR_TEXT "recv() function failed"
#define ERRSEND_ERROR_TEXT "send() function failed"
#define ERRPOLL_ERROR_TEXT "poll() function failed"
#define ERRTIME_ERROR_TEXT "time() function failed"
#define EMALLOC_ERROR_TEXT "malloc() function failed"
#define ERRLSTN_ERROR_TEXT "listen() function failed"
#define ETHREAD_ERROR_TEXT "pthread's function failed"
#define INVSOCK_ERROR_TEXT "invalid socket fd"
#define INVPATH_ERROR_TEXT "invalid file path"
#define TESTVAL_ERROR_TEXT "testval"
#define UNKNOWN_ERROR_TEXT "unknown error"

static inline void raise_failure_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(FAILURE), FAILURE_ERROR_TEXT, strerror(errno), func);
	LOGE("(FAILURE)" " " FAILURE_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_timeout_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(TIMEOUT), TIMEOUT_ERROR_TEXT, strerror(errno), func);
	LOGE("(TIMEOUT)" " " TIMEOUT_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_lowsize_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(LOWSIZE), LOWSIZE_ERROR_TEXT, strerror(errno), func);
	LOGE("(LOWSIZE)" " " LOWSIZE_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_exptry0_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(EXPTRY0), EXPTRY0_ERROR_TEXT, strerror(errno), func);
	LOGE("(EXPTRY0)" " " EXPTRY0_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_exptry1_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(EXPTRY1), EXPTRY1_ERROR_TEXT, strerror(errno), func);
	LOGE("(EXPTRY1)" " " EXPTRY1_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_zeroack_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ZEROACK), ZEROACK_ERROR_TEXT, strerror(errno), func);
	LOGE("(ZEROACK)" " " ZEROACK_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_zeroseq_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ZEROSEQ), ZEROSEQ_ERROR_TEXT, strerror(errno), func);
	LOGE("(ZEROSEQ)" " " ZEROSEQ_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_failset_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(FAILSET), FAILSET_ERROR_TEXT, strerror(errno), func);
	LOGE("(FAILSET)" " " FAILSET_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_failcrc_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(FAILCRC), FAILCRC_ERROR_TEXT, strerror(errno), func);
	LOGE("(FAILCRC)" " " FAILCRC_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_faillog_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(FAILLOG), FAILLOG_ERROR_TEXT, strerror(errno), func);
	LOGE("(FAILLOG)" " " FAILLOG_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badconf_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(BADCONF), BADCONF_ERROR_TEXT, strerror(errno), func);
	LOGE("(BADCONF)" " " BADCONF_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badflow_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(BADFLOW), BADFLOW_ERROR_TEXT, strerror(errno), func);
	LOGE("(BADFLOW)" " " BADFLOW_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badargs_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(BADARGS), BADARGS_ERROR_TEXT, strerror(errno), func);
	LOGE("(BADARGS)" " " BADARGS_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badinet_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(BADINET), BADINET_ERROR_TEXT, strerror(errno), func);
	LOGE("(BADINET)" " " BADINET_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badtype_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(BADTYPE), BADTYPE_ERROR_TEXT, strerror(errno), func);
	LOGE("(BADTYPE)" " " BADTYPE_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badipv4_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(BADIPV4), BADIPV4_ERROR_TEXT, strerror(errno), func);
	LOGE("(BADIPV4)" " " BADIPV4_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badport_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(BADPORT), BADPORT_ERROR_TEXT, strerror(errno), func);
	LOGE("(BADPORT)" " " BADPORT_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_nomfile_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(NOMFILE), NOMFILE_ERROR_TEXT, strerror(errno), func);
	LOGE("(NOMFILE)" " " NOMFILE_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_nofstat_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(NOFSTAT), NOFSTAT_ERROR_TEXT, strerror(errno), func);
	LOGE("(NOFSTAT)" " " NOFSTAT_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_noavail_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(NOAVAIL), NOAVAIL_ERROR_TEXT, strerror(errno), func);
	LOGE("(NOAVAIL)" " " NOAVAIL_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_nocreat_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(NOCREAT), NOCREAT_ERROR_TEXT, strerror(errno), func);
	LOGE("(NOCREAT)" " " NOCREAT_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_notrunc_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(NOTRUNC), NOTRUNC_ERROR_TEXT, strerror(errno), func);
	LOGE("(NOTRUNC)" " " NOTRUNC_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_noclose_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(NOCLOSE), NOCLOSE_ERROR_TEXT, strerror(errno), func);
	LOGE("(NOCLOSE)" " " NOCLOSE_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errbind_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ERRBIND), ERRBIND_ERROR_TEXT, strerror(errno), func);
	LOGE("(ERRBIND)" " " ERRBIND_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errconn_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ERRCONN), ERRCONN_ERROR_TEXT, strerror(errno), func);
	LOGE("(ERRCONN)" " " ERRCONN_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errrecv_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ERRRECV), ERRRECV_ERROR_TEXT, strerror(errno), func);
	LOGE("(ERRRECV)" " " ERRRECV_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errsend_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ERRSEND), ERRSEND_ERROR_TEXT, strerror(errno), func);
	LOGE("(ERRSEND)" " " ERRSEND_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errlstn_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ERRLSTN), ERRLSTN_ERROR_TEXT, strerror(errno), func);
	LOGE("(ERRLSTN)" " " ERRLSTN_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errpoll_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ERRPOLL), ERRPOLL_ERROR_TEXT, strerror(errno), func);
	LOGE("(ERRPOLL)" " " ERRPOLL_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errtime_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ERRTIME), ERRTIME_ERROR_TEXT, strerror(errno), func);
	LOGE("(ERRTIME)" " " ERRTIME_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_emfseek_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(EMFSEEK), EMFSEEK_ERROR_TEXT, strerror(errno), func);
	LOGE("(EMFSEEK)" " " EMFSEEK_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_emfsync_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(EMFSYNC), EMFSYNC_ERROR_TEXT, strerror(errno), func);
	LOGE("(EMFSYNC)" " " EMFSEEK_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_emalloc_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(EMALLOC), EMALLOC_ERROR_TEXT, strerror(errno), func);
	LOGE("(EMALLOC)" " " EMALLOC_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_ethread_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(ETHREAD), ETHREAD_ERROR_TEXT, strerror(errno), func);
	LOGE("(ETHREAD)" " " ETHREAD_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_invsock_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(INVSOCK), INVSOCK_ERROR_TEXT, strerror(errno), func);
	LOGE("(INVSOCK)" " " INVSOCK_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_invpath_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(INVPATH), INVPATH_ERROR_TEXT, strerror(errno), func);
	LOGE("(INVPATH)" " " INVPATH_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_testval_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(TESTVAL), TESTVAL_ERROR_TEXT, strerror(errno), func);
	LOGE("(TESTVAL)" " " TESTVAL_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_unknown_error(const char *func)
{
	fprintf(stderr, GLOBAL_ERROR_TEXT(UNKNOWN), UNKNOWN_ERROR_TEXT, strerror(errno), func);
	LOGE("(UNKNOWN)" " " UNKNOWN_ERROR_TEXT);
	exit(EXIT_FAILURE);
	return;
}

#endif
