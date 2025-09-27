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
    do {if (val != SUCCESS) {stat = val; return stat;}} while (0)
#define CHECK_INT(val, err)\
    do {if (val == -1) {stat = err; return stat;}} while (0)
#define CHECK_PTR(val, err)\
    do {if (val == NULL) {stat = err; return stat;}} while (0)
#define CHECK_BOOL(val, err)\
    do {if (val == false) {stat = err; return stat;}} while (0)
#define CHECK_NOTEOF(val, err)\
    do {if (val == EOF) {stat = err; return stat;}} while (0)
#define CHECK_MMAP(val)\
    do {if (val == MAP_FAILED) {stat = INVMMAP; return stat;}} while (0)
#define CHECK_IPV4(ip)\
    do {if (strlen(ip) + 1 != INET_ADDRSTRLEN) {stat = BADARGS; return stat;}} while (0)
#define CHECK_PORT(port)\
    do {if (port == 0) {stat = BADARGS; return stat;}} while (0)

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

typedef enum {
    SUCCESS,
    FAILURE,
    TIMEOUT,
    BADARGS,
    EMALLOC,
    BADTYPE,
    INVMMAP,
    TESTVAL
} status_t;

typedef unsigned char * Buffer;

typedef unsigned int sockfd_t;

typedef char * ipv4str_t;

typedef uint16_t port_t;

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
    Buffer buf;
} MFILE;

typedef struct {
    unsigned long start_pos;
    size_t chunk_size;
    status_t (*cread)(MFILE *mfile, size_t *total_size, Buffer buf, size_t len);
    void (*reset)(MFILE *mfile, unsigned long pos);
} ChunkContext;

typedef struct {
    MFILE *mfile;
    size_t size;
    ChunkContext (*get_chunk)(unsigned long start_pos, size_t chunk_size);
} FileContext;

#endif
