#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include "types.h"
#include "mfile.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

status_t start_file_stream(FileContext* filec, const char path[], fmode_t mode);

status_t end_file_stream(FileContext* filec);

#endif
