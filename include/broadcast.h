#ifndef RUFSHARE_BROADCAST_H
#define RUFSHARE_BROADCAST_H

#include "types.h"
#include "net_stream.h"
#include "utils/infostr.h"
#include "utils/sstr.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

status_t start_broadcast(void);

#endif
