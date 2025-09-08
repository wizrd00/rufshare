#include "logger/logger.h"

FILE logfile;

bool create_file(void) {
    unsigned char filename[FILENAME_MAX];
    sprintf(filename, "logfile_%ld.log", (long) time(NULL));
    logfile = fopen(filename, "w");
    if (logfile == NULL)
        fprintf(stderr, "Error(logger) : %s, %s\n\n", strerror(errno), filename);
    return (logfile != NULL) ? true : false;
}

bool start_logging(void) {
    return create_file();
}

void end_logging(void) {
    fclose(logfile);
}
// TODO
void log(void) {
    Log tmp_log;
    unsigned char date[11];
    unsigned char clock[16];
    strftime(date, sizeof (date), "%Y-%m-%d", tmp_log.time);
    strftime(clock, sizeof (clock), "%H:%M:%S.%f", tmp_log.time);
    append_log(count, tmp_log.level, date, clock, tmp_log.ver, tmp_log.mod, tmp_log.pos, tmp_log.msg);
    return;
}
