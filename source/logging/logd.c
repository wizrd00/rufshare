#include "logging/logd.h"

LogContext logc;

static char *sstrncpy(char *dst, const char *src, size_t dsize)
{
	strncpy(dst, src, dsize - 1);
	dst[dsize - 1] = '\0';
	return dst;
}

int init_logd(void)
{
	char filename[FILENAME_MAX];
	time_t time_tag = time(NULL);
	struct mq_attr attr = {.mq_maxmsg = LOGMAXMSG, .mq_msgsize = LOGMSGSIZE};
	logc.logcount = 0;
	snprintf(filename, FILENAME_MAX, "logfile_%ld.log", (time_tag != -1) ? (long) time_tag : (long) (rand() % 0xffff));
	logc.logfile = fopen(filename, "w");
	logc.logqueue = mq_open(LOGQUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, S_IRWXU, &attr);
	if ((logc.logfile == NULL) || (logc.logqueue == -1)) {
		fprintf(stderr, LOGERROR_TEXT, strerror(errno), "failed to create logger context");
		return -1;
	}
	return 0;
}

int start_logd(void)
{
	LogMsg logmsg;
	while (1) {
		if (mq_receive(logc.logqueue, (char *) &logmsg, LOGMSGSIZE, NULL) == -1) {
			sleep(SLEEPTIME);
			continue;
		}
		append_log(logc.logcount, logmsg.level, logmsg.date, logmsg.clock, logmsg.mod, logmsg.pos, logmsg.msg);
	}
	return 0;
}

int end_logd(void)
{
	return (fclose(logc.logfile) + mq_close(logc.logqueue) == 0) ? 0 : -1;
}

void logging(const char *level, const char *mod, const char *pos, const char *fmt, ...)
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
	strftime(logmsg.clock, CLOCKSIZE, "%H:%M:%S", ltime);
	sstrncpy(logmsg.mod, mod, MODSIZE);
	sstrncpy(logmsg.pos, pos, POSSIZE);
	va_start(ap, fmt);
	vsnprintf(msg, sizeof (msg), fmt, ap);
	sstrncpy(logmsg.msg, msg, MSGSIZE);
	va_end(ap);
	if (mq_send(logc.logqueue, (const char *) &logmsg, sizeof (logmsg), 0) == -1)
		fprintf(stderr, LOGERROR_TEXT, strerror(errno), (errno == EAGAIN) ? "log queue is full and new log dropped" : "mq_send() failed");
	logc.logcount++;
	return;
}
