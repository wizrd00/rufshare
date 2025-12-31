#include "logging/logd.h"

LogContext logc;

static char *sstrncpy(char *dst, const char *src, size_t dsize)
{
	strncpy(dst, src, dsize - 1);
	dst[dsize - 1] = '\0';
	return dst;
}

static int create_logfile(const char *path)
{
	size_t pathsize = strlen(path);
	char *logfile_path = (char *) calloc(pathsize + LOGFILE_NAMESIZE, sizeof (char));
	time_t logtime = time(NULL);
	if ((logtime == -1) || (logfile_path == NULL))
		return -1;
	sstrncpy(logfile_path, path, pathsize);
	snprintf(logfile_path + pathsize, LOGFILE_NAMESIZE, "logfile_%lu.log", (unsigned long) logtime);
	logc.logfile = fopen(logfile_path, "w+");
	if (logc.logfile == NULL)
		return -1;
	if (ftruncate(fileno(logc.logfile), LOGFILE_FILESIZE) == -1) {
		fclose(logc.logfile);
		return -1;
	}
	return 0;
}

static int map_logfile(void)
{
	logc.buffer = mmap(NULL, LOGFILE_FILESIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fileno(logc.logfile), 0);
	if (logc.buffer == MAP_FAILED) {
		fclose(logc.logfile);
		return -1;
	}
	logc.size = LOGFILE_FILESIZE;
	logc.pos = logc.logcount = 0;
	return 0;
}

int init_logd(const char *path)
{
	if (create_logfile(path) == -1)
		return -1;
	if (map_logfile() == -1)
		return -1;
	return 0;
}

int deinit_logd(void)
{
	return (fclose(logc.logfile) + munmap(logc.buffer, logc.size) == 0) ? 0 : -1;
}

void logging(const char *level, const char *mod, const char *pos, const char *fmt, ...)
{
	LogMsg logmsg;
	time_t logtime = time(NULL);
	struct tm *ltime = localtime(&logtime);
	char msg[MSGSIZE];
	va_list ap;
	if (ltime == NULL)
		return;
	sstrncpy(logmsg.level, level, LEVELSIZE);
	strftime(logmsg.date, DATESIZE, "%Y-%m-%d", ltime);
	strftime(logmsg.clock, CLOCKSIZE, "%H:%M:%S", ltime);
	sstrncpy(logmsg.mod, mod, MODSIZE);
	sstrncpy(logmsg.pos, pos, POSSIZE);
	va_start(ap, fmt);
	vsnprintf(msg, sizeof (msg), fmt, ap);
	sstrncpy(logmsg.msg, msg, MSGSIZE);
	va_end(ap);
	logc.logcount++;
	append_log(logmsg);
	return;
}
