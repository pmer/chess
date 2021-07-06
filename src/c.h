#ifndef C_H_
#define C_H_

#include "util/int.h"

extern "C" {
// bits/alltypes.h
typedef U16 in_port_t;
typedef U16 sa_family_t;
typedef U32 socklen_t;

// netinet/in.h
typedef U32 in_addr_t;
struct in_addr {
    in_addr_t s_addr;
};
struct sockaddr_in {
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;
    U8 sin_zero[8];
};
U16
htons(U16) noexcept;

// sys/socket.h
#define AF_INET 2
#define INADDR_ANY 0
#define SOCK_STREAM 1
#define SOL_SOCKET 1
#define SO_REUSEADDR 2
#define SO_REUSEPORT 15
struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};
int
accept(int, struct sockaddr*, socklen_t*) noexcept;
int
bind(int, const struct sockaddr*, socklen_t) noexcept;
int
listen(int, int) noexcept;
int
setsockopt(int, int, int, const void*, socklen_t) noexcept;
int
socket(int, int, int) noexcept;
}

#endif  // C_H_
