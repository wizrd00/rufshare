#ifndef RUFSHARE_LOGD_H
#define RUFSHARE_LOGD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
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
#define CLOCKSIZE 9
#define MODSIZE 32
#define POSSIZE 32
#define MSGSIZE 512
#define SLEEPTIME 1

#define append_log(count, level, date, clock, mod, pos, fmt)\
	do {fprintf(logc.logfile, "[0x%lx][%-5.5s][%-10.10s][%-8.8s][MOD:%-.31s][POS:%-.31s][%-.511s]\n", count, level, date, clock, mod, pos, fmt);} while (0)

int start_logd(void);

void end_logd(void);

void logging(const unsigned long *count, const char *level, const char *mod, const char *pos, const char *fmt, ...);

typedef struct {
	char level[LEVELSIZE];
	char date[DATESIZE];
	char clock[CLOCKSIZE];
	char mod[MODSIZE];
	char pos[POSSIZE];
	char msg[MSGSIZE];
} LogMsg;

typedef struct {
	FILE *logfile;
	mqd_t logqueue;
	unsigned long logcount;
} LogContext;

extern LogContext logc;

#endif
