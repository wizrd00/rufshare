#include "logger/logger.h"

FILE* logfile;
unsigned long logcount;

signed int start_logging(void) {
    unsigned char filename[FILENAME_MAX];
    sprintf(filename, "logfile_%ld.log", (long) time(NULL));
    logfile = fopen((char *) filename, "w");
    if (logfile == NULL)
        fprintf(stderr, "Error(logger) : %s, %s\n\n", strerror(errno), filename);
    return (logfile != NULL) ? 0 : -1;
}

void end_logging(void) {
    fclose(logfile);
}

void logging(
    const unsigned long count,
    const unsigned char* level,
    const unsigned char* mod,
    const unsigned char* pos,
    const unsigned char* msg
) {
    unsigned char date[11];
    unsigned char clock[16];
    strftime(date, sizeof (date), "%Y-%m-%d", tmp_log.time);
    strftime(clock, sizeof (clock), "%H:%M:%S.%f", tmp_log.time);
    append_log(*count, tmp_log.level, date, clock, tmp_log.mod, tmp_log.pos, tmp_log.msg);
    (*count)++;
    return;
}
