#include "logger/logd.h"

FILE *logfile;
unsigned long logcount;
mqd_t logqueue;

int start_logd(void) {
	char filename[FILENAME_MAX];
	time_t time_tag = time(NULL);
	snprintf(filename, FILENAME_MAX, "logfile_%ld.log", (time_tag != -1) ? (long) time_tag : (long) (rand() % 0xffff));
	logfile = fopen(filename, "w");
	logqueue = mq_open(LOGQUEUE_NAME, O_RDWR);	
	if ((logfile == NULL) || (logqueue == -1)) {
		fprintf(stderr, LOGERROR_TEXT, strerror(errno), "failed to create logger context");
		return -1;
	}
	return 0;
}

void end_logging(void) {
	fclose(logfile);
}

void logging(
	const unsigned long *count,
	const unsigned char *level,
	const unsigned char *mod,
	const unsigned char *pos,
	const unsigned char *fmt,
	...
) {
	char date[11], clock[16], msg[128];
	struct tm ltime = localtime(time(NULL));
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, sizeof (msg), fmt, ap);
	strftime(date, sizeof (date), "%Y-%m-%d", &ltime);
	strftime(clock, sizeof (clock), "%H:%M:%S.%f", &ltime);
	append_log(*count, level, date, clock, mod, pos, msg);
	va_end(ap);
	(*count)++;
	return;
}
