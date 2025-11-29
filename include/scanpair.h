#ifndef SCANPAIR_H
#define SCANPAIR_H

#include "types.h"
#include "utils/infostr.h"
#include "utils/sstr.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <poll.h>

status_t start_scanpair(PairInfo *info, size_t *len);

#endif
