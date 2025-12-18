#ifndef RUFSHARE_IPCHECK_H
#define RUFSHARE_IPCHECK_H

#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>

static inline bool check_ipv4_format(const char *ip)
{
    uint32_t ipnetorder;
    if (inet_pton(AF_INET, ip, &ipnetorder) != 1)
        return false;
    return true;
}

#endif
