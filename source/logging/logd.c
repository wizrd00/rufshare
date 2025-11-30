#include "logging/logd.h"

LogContext logc;

static char *sstrncpy(char *dst, const char *src, size_t dsize)
{
	strncpy(dst, src, dsize - 1);
	dst[dsize - 1] = '\0';
	return dst;
}

int start_logd(void)
{
	char filename[FILENAME_MAX];
	time_t time_tag = time(NULL);
	LogMsg logmsg;
	logc.logcount = 0;
	snprintf(filename, FILENAME_MAX, "logfile_%ld.log", (time_tag != -1) ? (long) time_tag : (long) (rand() % 0xffff));
	logc.logfile = fopen(filename, "w");
	logc.logqueue = mq_open(LOGQUEUE_NAME, O_RDWR);	
	if ((logc.logfile == NULL) || (logc.logqueue == -1)) {
		fprintf(stderr, LOGERROR_TEXT, strerror(errno), "failed to create logger context");
		return -1;
	}
	while (1) {
		if (mq_receive(logc.logqueue, (char *) &logmsg, sizeof (LogMsg), NULL) != sizeof (LogMsg))
			return -1;
		append_log(logc.logcount, logmsg.level, logmsg.date, logmsg.clock, logmsg.mod, logmsg.pos, logmsg.msg);
	}
	return 0;
}

void end_logd(void)
{
	fclose(logc.logfile);
	mq_close(logc.logqueue);
	return;
}

void logging(const unsigned long *count, const char *level, const char *mod, const char *pos, const char *fmt, ...)
{
	LogMsg logmsg;
	time_t now_time = time(NULL);
	struct tm *ltime = localtime(&now_time);
	if (ltime == NULL)
		return;
	char msg[MSGSIZE];
	va_list ap;
	sstrncpy(logmsg.level, level, LEVELSIZE);
	strftime(logmsg.date, DATESIZE, "%Y-%m-%d", ltime);
	strftime(logmsg.clock, CLOCKSIZE, "%H:%M:%S.%f", ltime);
	sstrncpy(logmsg.mod, mod, MODSIZE);
	sstrncpy(logmsg.pos, pos, POSSIZE);
	va_start(ap, fmt);
	vsnprintf(msg, sizeof (msg), fmt, ap);
	sstrncpy(logmsg.msg, msg, MSGSIZE);
	va_end(ap);
	mq_send(logc.logqueue, (const char *) &logmsg, sizeof (logmsg), 0);
	(*count)++;
	return;
}
