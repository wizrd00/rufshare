#include "logging/logd.h"

LogContext logc;

static char *sstrncpy(char *dst, const char *src, size_t dsize)
{
	size_t dlen;
	for (dlen = 0; (dlen < dsize) && (src[dlen] != '\0'); dlen++);
	memcpy(dst, src, dlen);
	dst[(dsize == dlen) ? dsize - 1 : dlen] = '\0';
	return dst;
}

static int create_logfile(const char *path)
{
	size_t pathlen = strlen(path);
	char *logfile_path = (char *) calloc(pathlen + LOGFILE_NAMESIZE + 1, sizeof (char));
	time_t logtime = time(NULL);
	if ((logtime == -1) || (logfile_path == NULL))
		return -1;
	sstrncpy(logfile_path, path, pathlen + 1);
	snprintf(logfile_path + pathlen, LOGFILE_NAMESIZE, "logfile_%u.log", (unsigned int) logtime);
	logc.logfile = fopen(logfile_path, "w+");
	if (logc.logfile == NULL)
		return -1;
	if (ftruncate(fileno(logc.logfile), (off_t) logc.size) == -1) {
		fclose(logc.logfile);
		return -1;
	}
	return 0;
}

static int map_logfile(void)
{
	logc.buffer = mmap(NULL, logc.size, PROT_WRITE | PROT_READ, MAP_SHARED, fileno(logc.logfile), 0);
	if (logc.buffer == MAP_FAILED) {
		fclose(logc.logfile);
		return -1;
	}
	logc.pos = logc.logcount = 0;
	return 0;
}

int init_logd(const char *path, size_t count)
{
	logc.size = count * sizeof (LogMsg);
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
