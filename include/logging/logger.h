#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define TRACE "TRACE"
#define DEBUG "DEBUG"
#define WARNN "WARNN"
#define ERROR "ERROR"

#define append_log(count, level, date, clock, mod, pos, msg)\
    do {\
        fprintf(\
            &logfile,\
            "[0x%lx][%-5.5s][%-10.10s][%-15.15s][MOD:%-.32s][POS:%-.32s][%-.64s]\n",\
            count, level, date, clock, mod, pos, msg\
        );\
    } while (0)

extern FILE* logfile;
extern unsigned long logcount;

signed int start_logging(void);

void end_logging(void);

void logging(
    const unsigned long* count,
    const unsigned char* level,
    const unsigned char* ver,
    const unsigned char* mod,
    const unsigned char* pos,
    const unsigned char* msg
);

#endif
