#ifndef INFOSTR_H
#define INFOSTR_H

#include "types.h"
#include <stdio.h>

#define INFOSTRSIZE 320 * sizeof (char)
#define INFOSTRFORMAT "%.MAXFILENAMELENs:%.MAXNAMELENs@%.MAXIPV4LENs:%.MAXPORTLENhu"

static inline void pack_into_infostring(char *infostr, CntlAddrs *info) {
    snprintf(infostr, INFOSTRSIZE, INFOSTRFORMAT, info->filename, info->name, info->local_ip, info->local_port);
    return;
}

static inline void unpack_from_infostring(char *infostr, CntlAddrs *info) {
    sscanf(infostr, INFOSTRFORMAT, info->filename, info->name, info->remote_ip, info->remote_port);
    return;
}

#endif
