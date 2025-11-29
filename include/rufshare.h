#ifndef RUFSHARE_H
#define RUFSHARE_H

#include "types.h"
#include "file_stream.h"
#include "cntl.h"
#include "data.h"
#include "pusher.h"
#include "puller.h"
#include "broadcast.h"
#include "scanpair.h"
#include "tryexec.h"
#include "utils/calc.h"
#include "utils/fname.h"
#include "utils/sstr.h"
#include <string.h>
#include <pthread.h>

status_t push_file(const char *path);

status_t pull_file(const char *path, char *remote_name);

status_t broadcast(void);

status_t scanpair(PairInfo *info, size_t *len);

extern InitConfig *conf;

#endif
