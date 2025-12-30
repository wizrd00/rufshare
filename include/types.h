#ifndef RUFSHARE_TYPES_H
#define RUFSHARE_TYPES_H

#if defined(LOG_TRACE) || defined(LOG_DEBUG) || defined(LOG_WARNN) || defined(LOG_ERROR)
#define LOGGING
#include "logging/logd.h"
#endif
#include "protocol/protocol.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>

#define FOREVER_TIMEOUT -1
#define MAXFILENAMESIZE 256
#define MAXNAMESIZE 32
#define MAXIPV4SIZE 16

#ifdef LOG_TRACE
	#define LOGT(...) logging(TRACE, __FILE__, __func__, __VA_ARGS__)
#else
	#define LOGT(...)
#endif

#ifdef LOG_DEBUG
	#define LOGD(...) logging(DEBUG, __FILE__, __func__, __VA_ARGS__)
#else
	#define LOGD(...)
#endif

#ifdef LOG_WARNN
	#define LOGW(...) logging(WARNN, __FILE__, __func__, __VA_ARGS__)
#else
	#define LOGW(...)
#endif

#ifdef LOG_ERROR
	#define LOGE(...) logging(ERROR, __FILE__, __func__, __VA_ARGS__)
#else
	#define LOGE(...)
#endif

#define CHECK_STAT(val, ...)\
	do {if (val != SUCCESS) {LOGE("(" #val ")" " " __VA_ARGS__); return _stat = val;}} while (0)

#define CHECK_SSTAT(val, ptr, ...)\
	do {if (val != SUCCESS) {LOGE("(" #val ")" " " __VA_ARGS__); free(ptr); return _stat = val;}} while (0)

#define CHECK_EQUAL(val0, val1, err, ...)\
	do {if (val0 != val1) {LOGE("(" #err ")" " " __VA_ARGS__); return _stat = err;}} while (0)

#define CHECK_NOTEQUAL(val0, val1, err, ...)\
	do {if (val0 == val1) {LOGE("(" #err ")" " " __VA_ARGS__); return _stat = err;}} while (0)

#define CHECK_INT(val, err, ...)\
	do {if (val == -1) {LOGE("(" #err ")" " " __VA_ARGS__); return _stat = err;}} while (0)

#define CHECK_PTR(val, err, ...)\
	do {if (val == NULL) {LOGE("(" #err ")" " " __VA_ARGS__); return _stat = err;}} while (0)

#define CHECK_BOOL(val, err, ...)\
	do {if (val == false) {LOGE("(" #err ")" " " __VA_ARGS__); return _stat = err;}} while (0)

#define CHECK_SIZE(val, size, ...)\
	do {if (val < size) {LOGE("(" "LOWSIZE" ")" " " __VA_ARGS__); return _stat = LOWSIZE;}} while (0)

#define CHECK_IPV4(ip, ...)\
	do {if (!check_ipv4_format(ip)) {LOGE("(" "BADIPV4" ")" " " __VA_ARGS__); return _stat = BADIPV4;}} while (0)

#define CHECK_PORT(port, ...)\
	do {if (port == 0) {LOGE("(" "BADPORT" ")" " " __VA_ARGS__); return _stat = BADPORT;}} while (0)

#define CHECK_MFILE(mfile, ...)\
	do {if (mfile.open == 0) {LOGE("(" "NOMFILE" ")" " " __VA_ARGS__); return _stat = NOMFILE;}} while (0)

#define CHECK_THREAD(val, ...)\
	do {if (val != 0) {LOGE("(" "ETHREAD" ")" " " __VA_ARGS__); return _stat = ETHREAD;}} while (0)

typedef unsigned char * buffer_t;

typedef int sockfd_t;

typedef char * ipv4str_t;

typedef uint16_t port_t;

typedef enum {
	SUCCESS,
	FAILURE,
	TIMEOUT,
	LOWSIZE,
	EXPTRY0,
	EXPTRY1,
	ZEROACK,
	ZEROSEQ,
	ZEROCHK,
	FAILSET,
	FAILCRC,
	FAILLOG,
	BADCONF,
	BADFLOW,
	BADARGS,
	BADINET,
	BADTYPE,
	BADIPV4,
	BADPORT,
	NOMFILE,
	NOFSTAT,
	NOAVAIL,
	NOCREAT,
	NOTRUNC,
	NOCLOSE,
	ERRBIND,
	ERRCONN,
	ERRRECV,
	ERRSEND,
	ERRLSTN,
	ERRPOLL,
	ERRTIME,
	EMALLOC,
	ETHREAD,
	INVSOCK,
	INVPATH,
	TESTVAL
} status_t;

typedef enum {
	MWR,
	MRD
} fmode_t;

typedef struct {
	char *ip;
	unsigned short port;
} addr_pair;

typedef struct {
	char filename[MAXFILENAMESIZE];
	char name[MAXNAMESIZE];
	char local_ip[MAXIPV4SIZE];
	char remote_ip[MAXIPV4SIZE];
	unsigned short local_port;
	unsigned short remote_port;
} CntlAddrs;

typedef union {
	struct {
		CastPacket packet;
		CntlAddrs info;
	} cast;
	struct {
		FlowPacket packet;
		CntlAddrs info;
	} flow;
	struct {
		SendPacket packet;
		CntlAddrs info;
	} send;
	struct {
		RecvPacket packet;
		CntlAddrs info;
	} recv;
} HeaderArgs;

typedef struct {
	FILE *file;
	short open;
	size_t size;
	size_t pos;
	void *buf;
} MFILE;

typedef struct {
	MFILE mfile;
	size_t size;
	char name[MAXFILENAMESIZE];
} FileContext;

typedef struct {
	unsigned long start_pos;
	size_t chunk_size;
} ChunkContext;

typedef struct {
	char name[MAXNAMESIZE];
	addr_pair addr;
} PairInfo;

typedef struct {
	RUFShareChunkSize chsize;
	RUFSharePartialChunkSize pchsize;
	RUFShareChunkCount chcount;
	RUFShareSequence seq;
	sockfd_t cntl_sock;
	sockfd_t data_sock;
	sockfd_t conn_sock;
	sockfd_t cast_sock;
	FileContext filec;
	CntlAddrs addrs;
	size_t segsize;
	int hst_send;
	int hst_recv;
	int vft_send;
	int vft_recv;
	int tft_flow;
	int tft_recv;
	int tft_data;
	int bct_cast;
	int spt_cast;
	short bc_interval;
	short sp_interval;
	short tf_trycount;
	short bc_trycount;
	short sp_trycount;
} InitConfig;

extern InitConfig *conf;

#endif
