#include "utils/getdir.h"

unsigned char *get_dir(const signed char *path, signed char *dir) {
    size_t len = strlen(path) + 1;
    strcpy(dir, path);
    for (int i = len; i > 0; i--) {
        dir[i] = '\0';
        if (dir[i - 1] == '/')
            break;
    }
    if (i == 1)
        strcpy(dir, "./");
    return dir;
}
