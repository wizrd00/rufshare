#ifndef TYPES_H
#define TYPES_H

#if defined(LOG_TRACE) || defined(LOG_DEBUG) || defined(OG_WARNN) || defined(LOG_ERROR)
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

#define SEGMENTSIZE 512 * sizeof (char)
#define MAXFILENAMESIZE 256 * sizeof (char)
#define MAXFILENAMELEN 255
#define MAXNAMESIZE 32 * sizeof (char)
#define MAXNAMELEN 31
#define MAXPORTLEN 5
#define MAXIPV4SIZE 16 * sizeof (char)
#define MAXIPV4LEN 15
#define FOREVER_TIMEOUT -1
#define HANDSHAKE_SEND_TIMEOUT 8 * 1000
#define HANDSHAKE_RECV_TIMEOUT 8 * 1000
#define VERIFICATION_SEND_TIMEOUT 8 * 1000
#define VERIFICATION_RECV_TIMEOUT 8 * 1000
#define TRANSFER_FLOW_TIMEOUT 8 * 1000
#define TRANSFER_RECV_TIMEOUT 8 * 1000
#define TRANSFER_DATA_TIMEOUT 1 * 1000
#define BROADCAST_CAST_TIMEOUT 1 * 1000
#define TRANSFER_TRY_COUNT 3
#define BROADCAST_TRY_COUNT 3
#define BROADCAST_INTERVAL 2
#define BROADCAST_IPV4 "255.255.255.255"

#define CHECK_SSTAT(val, ptr)\
	do {if (val != SUCCESS) {free(buf); return _stat = val;}} while (0)
#define CHECK_STAT(val)\
	do {if (val != SUCCESS) {return _stat = val;}} while (0)

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

typedef unsigned char * Buffer;

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
	FAILSET,
	FAILCRC,
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
	unsigned char *ip;
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
	int handle;
} BroadCast;

#endif
