#ifndef RUFSHARE_H
#define RUFSHARE_H

#include "types.h"
#include "file_stream.h"
#include "cntl.h"
#include "data.h"
#include "error.h"
#include "utils/calc.h"
#include "utils/fname.h"
#include <string.h>

status_t push_file(const char *path, addr_pair *local, addr_pair *remote);

status_t pull_file(const char *path, addr_pair *local, addr_pair *remote);

status_t scan_pair(PairInfo *info, addr_pair *local);

#endif
