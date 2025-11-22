#ifndef LOGD_H
#define LOGD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>

#define LOGQUEUE_NAME "logqueue"
#define LOGERROR_TEXT "\nError(logger) : %s, %s\n\n"

#define TRACE "TRACE"
#define DEBUG "DEBUG"
#define WARNN "WARNN"
#define ERROR "ERROR"

#define append_log(count, level, date, clock, mod, pos, fmt)\
	do {\
		fprintf(\
			&logfile,\
			"[0x%lx][%-5.5s][%-10.10s][%-15.15s][MOD:%-.32s][POS:%-.32s][%-.128s]\n",\
			count, level, date, clock, mod, pos, fmt\
		);\
	} while (0)

extern FILE *logfile;
extern unsigned long logcount;

int start_logging(void);

void end_logging(void);

void logging(
	const unsigned long *count,
	const char level[],
	const char ver[],
	const char mod[],
	const char pos[],
	const char fmt[],
	...
);

#endif
