#ifndef RUFSHARE_PULLER_H
#define RUFSHARE_PULLER_H

#include "types.h"
#include "cntl.h"
#include "data.h"
#include "file_stream.h"
#include "utils/calc.h"
#include <pthread.h>

#define ISVALID_SEND_HEADER(header) ((header.send.packet.chunk_size >= 0) || (header.send.packet.partial_chunk_size >= 0) || (header.send.packet.chunk_count > 0))

status_t start_puller(const char *path, char *remote_name);

#endif
