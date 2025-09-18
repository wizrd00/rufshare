#ifndef CNTL_H
#define CNTL_H

#include "types.h"
#include "net_stream.h"
#include "protocol/protocol.h"
#include "utils/infostr.h"
#include <poll.h>
#include <netinet/in.h>

typedef struct {
    unsigned char local_ip[INET_ADDRSTRLEN],
    unsigned char remote_ip[INET_ADDRSTRLEN],
    unsigned short local_port,
    unsigned short remote_port
} CntlAddrs;

typedef enum {
    struct cast {
        RUFShareCRC16 crc,
        local_info* info
    },
    struct flow {
        RUFShareChunkSize chunk_size,
        RUFShareSequence sequence,
        RUFShareCRC32 crc
    },
    struct send {
        RUFShareChunkSize chunk_size,
        RUFShareChunkCount chunk_count,
        RUFSharePartialChunkSize partial_chunk_size,
        RUFShareCRC16 crc,
        local_info* info
    },
    struct recv {
        RUFShareACK ack,
        RUFShareCRC16 crc,
        RUFShareSequence sequence
    }
} HeaderArgs;

status_t start_cntl(CntlAddrs* addr);

status_t push_header(RUFShareType type, HeaderArgs* args);

extern sockfd_t TCPsock;
extern time_t SNDtimeout;
extern time_t RCVtimeout;

#endif
