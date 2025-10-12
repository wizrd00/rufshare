#include "logger/logger.h"

FILE *logfile;
unsigned long logcount;

int start_logging(void) {
	char filename[FILENAME_MAX];
	snprintf(filename, FILENAME_MAX, "logfile_%ld.log", (long) time(NULL));
	logfile = fopen(filename, "w");
	if (logfile == NULL)
		fprintf(stderr, "Error(logger) : %s, %s\n\n", strerror(errno), filename);
	return (logfile != NULL) ? 0 : -1;
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
