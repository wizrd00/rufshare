#ifndef RUFSHARE_INFOSTR_H
#define RUFSHARE_INFOSTR_H

#include "types.h"
#include <stdio.h>

#define INFOSTRSIZE 320 * sizeof (char)

static inline void pack_into_infostring(char *infostr, CntlAddrs *info)
{
    snprintf(infostr, INFOSTRSIZE, "%.255s:%.31s@%.15s:%.5hu", info->filename, info->name, info->local_ip, info->local_port);
    return;
}

static inline void unpack_from_infostring(char *infostr, CntlAddrs *info)
{
    sscanf(infostr, "%255[^:]:%31[^@]@%15[^:]:%hu", info->filename, info->name, info->remote_ip, &(info->remote_port));
    return;
}

#endif
