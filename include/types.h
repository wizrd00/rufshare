#ifndef TYPES_H
#define TYPES_H

#if defined(LOG_TRACE) || defined(LOG_DEBUG) || defined(OG_WARNN) || defined(LOG_ERROR)
#define LOGGING
#include "logging/logger.h"
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

#define CHECK_STAT(val)\
	do {if (val != SUCCESS) {return _stat = val;}} while (0)

#define CHECK_SSTAT(val, ptr)\
	do {if (val != SUCCESS) {free(ptr); return _stat = val;}} while (0)

#define CHECK_EQUAL(val0, val1, err)\
	do {if (val0 != val1) {return _stat = err;}} while (0)

#define CHECK_NOTEQUAL(val0, val1, err)\
	do {if (val0 == val1) {return _stat = err;}} while (0)

#define CHECK_INT(val, err)\
	do {if (val == -1) {return _stat = err;}} while (0)

#define CHECK_PTR(val, err)\
	do {if (val == NULL) {return _stat = err;}} while (0)

#define CHECK_BOOL(val, err)\
	do {if (val == false) {return _stat = err;}} while (0)

#define CHECK_SIZE(val, size)\
	do {if (val < size) {return _stat = LOWSIZE;}} while (0)

#define CHECK_PORT(port)\
	do {if (port == 0) {return _stat = BADPORT;}} while (0)

#define CHECK_MFILE(mfile)\
	do {if (mfile.open == 0) {return _stat = NOMFILE;}} while (0)

#define CHECK_THREAD(val)\
	do {if (val != 0) {return _stat = ETHREAD;}} while (0)

#ifdef LOG_TRACE
	#define LOGT(mod, pos, ...) logging(&logcount, TRACE, mod, pos, __VA_ARGS__)
#else
	#define LOGT(mod, pos, ...)
#endif

#ifdef LOG_DEBUG
	#define LOGD(mod, pos, ...) logging(&logcount, DEBUG, mod, pos, __VA_ARGS__)
#else
	#define LOGD(mod, pos, ...)
#endif

#ifdef LOG_WARNN
	#define LOGW(mod, pos, ...) logging(&logcount, WARNN, mod, pos, __VA_ARGS__)
#else
	#define LOGW(mod, pos, ...)
#endif

#ifdef LOG_ERROR
	#define LOGE(mod, pos, ...) logging(&logcount, ERROR, mod, pos, __VA_ARGS__)
#else
	#define LOGE(mod, pos, ...)
#endif

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
	addr_pair bc_addr;
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
} InitConfig;

extern InitConfig *conf;

#endif
