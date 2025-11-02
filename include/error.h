#ifndef ERROR_H
#define ERROR_H

#include "types.h"
#include <stdio.h>
#include <unistd.h>

#define GLOBAL_ERROR_TEXT(stat) "\n[\x1b[1;31mERROR\x1b[0m] -> " #stat "\n\tstat -> %s" "\n\terrno -> %s (maybe not related)" "\n\tfunc -> %s()\n"
#define FAILURE_ERROR_TEXT "process failed"
#define TIMEOUT_ERROR_TEXT "process timed out"
#define LOWSIZE_ERROR_TEXT "process received size that is lower than expected" 
#define EXPTRY0_ERROR_TEXT "try count reached zero"
#define EXPTRY1_ERROR_TEXT "try count reached zero"
#define ZEROACK_ERROR_TEXT "process received RECV-NACK"
#define ZEROSEQ_ERROR_TEXT "transfer function received sequence = 0"
#define FAILSET_ERROR_TEXT "process failed to set socket option"
#define FAILPOS_ERROR_TEXT ""
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
#define ERRBIND_ERROR_TEXT "bind() function failed"
#define ERRCONN_ERROR_TEXT "connect() function failed"
#define ERRRECV_ERROR_TEXT "recv() function failed"
#define ERRSEND_ERROR_TEXT "send() function failed"
#define ERRPOLL_ERROR_TEXT "poll() function failed"
#define EMALLOC_ERROR_TEXT "malloc() function failed"
#define INVSOCK_ERROR_TEXT "invalid socket fd"
#define TESTVAL_ERROR_TEXT "testval"

static inline void raise_failure_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, FAILURE_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_timeout_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, TIMEOUT_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_lowsize_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, LOWSIZE_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_exptry0_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, EXPTRY0_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_exptry1_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, EXPTRY1_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_zeroack_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, ZEROACK_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_zeroseq_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, ZEROSEQ_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_failset_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, FAILSET_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_failpos_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, FAILPOS_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badargs_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, BADARGS_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badinet_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, BADINET_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badtype_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, BADTYPE_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badipv4_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, BADIPV4_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_badport_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, BADPORT_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_nomfile_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, NOMFILE_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_nofstat_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, NOFSTAT_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_noavial_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, NOAVAIL_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_nocreat_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, NOCREAT_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_notrunc_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, NOTRUNC_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_noclose_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, NOCLOSE_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errbind_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, ERRBIND_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errconn_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, ERRCONN_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errrecv_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, ERRRECV_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errsend_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, ERRSEND_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_errpoll_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, ERRPOLL_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_emalloc_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, EMALLOC_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_invsock_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, INVSOCK_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

static inline void raise_testval_error(void *func) {
	fprintf(stderr, GLOBAL_ERROR_TEXT, TESTVAL_ERROR_TEXT, func);
	exit(EXIT_FAILURE);
	return;
}

#endif
