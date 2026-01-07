#ifndef RUFSHARE_LOGD_H
#define RUFSHARE_LOGD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#define LOGERROR_TEXT "\nError(logger) : %s, %s\n\n"
#define LOGFILE_NAMESIZE 32
#define LOGFILE_FILESIZE sizeof (LogMsg) * 8192

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

#define append_log(log)\
	do {logc.pos -= (logc.pos == logc.size) ? logc.size : 0; memcpy((void *) ((char *) logc.buffer + logc.pos), (void *) &log, sizeof (LogMsg)); logc.pos += sizeof (LogMsg);} while (0)

int init_logd(const char *path);

int deinit_logd(void);

void logging(const char *level, const char *mod, const char *pos, const char *fmt, ...);

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
	void *buffer;
	size_t pos;
	size_t size;
	unsigned long logcount;
} LogContext;

#endif
