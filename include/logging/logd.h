#ifndef LOGD_H
#define LOGD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <mqueue.h>
#include <errno.h>

#define LOGQUEUE_NAME "logqueue"
#define LOGERROR_TEXT "\nError(logger) : %s, %s\n\n"

#define TRACE "TRACE"
#define DEBUG "DEBUG"
#define WARNN "WARNN"
#define ERROR "ERROR"

#define LEVELSIZE 6
#define DATESIZE 11
#define CLOCKSIZE 16
#define MODSIZE 32
#define POSSIZE 32
#define MSGSIZE 128

#define append_log(count, level, date, clock, mod, pos, fmt)\
	do {\
		fprintf(\
			&logfile,\
			"[0x%lx][%-5.5s][%-10.10s][%-15.15s][MOD:%-.31s][POS:%-.31s][%-.127s]\n",\
			count, level, date, clock, mod, pos, fmt\
		);\
	} while (0)

extern FILE *logfile;
extern unsigned long logcount;

int start_logging(void);

void end_logging(void);

void logging(
	const unsigned long *count,
	const char *level,
	const char *ver,
	const char *mod,
	const char *pos,
	const char *fmt,
	...
);

typedef struct {
	level[LEVELSIZE];
	date[DATESIZE];
	clock[CLOCKSIZE];
	mod[MODSIZE];
	pos[POSSIZE];
	msg[MSGSIZE];
} LogMsg;

typedef struct {
	FILE *logfile;
	mqd_t logqueue;
	unsigned long logcount;
} LogContext;

#endif
