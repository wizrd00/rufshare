#ifndef TYPES_H
#define TYPES_H

#include "protocol/protocol.h"
#include "logging/logger.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>

#define MAXFILENAMESIZE 256
#define MAXNAMESIZE 32

#define CHECK_STAT(val)\
    do {if (val != SUCCESS) {return stat = val;}} while (0)

#define CHECK_NOTEQUAL(val0, val1, err)\
    do {if (val0 != val1) {return stat = err;}} while (0)

#define CHECK_INT(val, err)\
    do {if (val == -1) {return stat = err;}} while (0)

#define CHECK_PTR(val, err)\
    do {if (val == NULL) {return stat = err;}} while (0)

#define CHECK_BOOL(val, err)\
    do {if (val == false) {return stat = err;}} while (0)

#define CHECK_SIZE(val, size)\
    do {if (val < size) {return stat = LOWSIZE;}} while (0)

#define CHECK_IPV4(ip)\
    do {if (strlen(ip) + 1 != INET_ADDRSTRLEN) {return stat = BADIPV4;}} while (0)

#define CHECK_PORT(port)\
    do {if (port == 0) {return stat = BADPORT;}} while (0)

#define CHECK_MFILE(mfile)\
    do {if ((mfile.file == NULL) || (mfile.buf == NULL)) {return stat = NOMFILE;}} while (0)

#ifdef LOG_TRACE
    #define LOGT(mod, pos, msg) logging(&logcount, TRACE, mod, pos, msg)
#else
    #define LOGT(mod, pos, msg)
#endif

#ifdef LOG_DEBUG
    #define LOGD(mod, pos, msg) logging(&logcount, DEBUG, mod, pos, msg)
#else
    #define LOGD(mod, pos, msg)
#endif

#ifdef LOG_WARNN
    #define LOGW(mod, pos, msg) logging(&logcount, WARNN, mod, pos, msg)
#else
    #define LOGW(mod, pos, msg)
#endif

#ifdef LOG_ERROR
    #define LOGE(mod, pos, msg) logging(&logcount, ERROR, mod, pos, msg)
#else
    #define LOGE(mod, pos, msg)
#endif

typedef unsigned char * Buffer;

typedef unsigned int sockfd_t;

typedef char * ipv4str_t;

typedef uint16_t port_t;

typedef enum {
    SUCCESS,
    FAILURE,
    TIMEOUT,
    BADARGS,
    EMALLOC,
    NOMFILE,
    NOFSTAT,
    NOAVAIL,
    NOCREAT,
    NOTRUNC,
    BADTYPE,
    LOWSIZE,
    BADIPV4,
    BADPORT,
    TESTVAL
} status_t;

typedef enum {
    MWR,
    MRD
} fmode_t;

typedef struct {
    char filename[MAXFILENAMESIZE];
    char name[MAXNAMESIZE];
    char local_ip[INET_ADDRSTRLEN];
    char remote_ip[INET_ADDRSTRLEN];
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
    size_t size;
    size_t pos;
    void *buf;
} MFILE;

typedef struct {
    unsigned long start_pos;
    size_t chunk_size;
} ChunkContext;

typedef struct {
    MFILE mfile;
    size_t size;
    char name[MAXFILENAMESIZE];
} FileContext;

#endif
