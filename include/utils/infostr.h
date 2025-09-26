#ifndef INFOSTR_H
#define INFOSTR_H

#include "types.h"

#define INFOSTRSIZE 312 * sizeof(char)

char *pack_into_infostring(char *infostr, CntlAddrs *info);
CntlAddrs unpack_from_infostring(char *infostr);

#endif
