#include "socket.hpp"

#include <stdexcept>


#ifdef __MINGW32__
int
inet_pton(int af, const char* src, void* dst)
{
    sockaddr_storage ss;
    int  size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    ::ZeroMemory(&ss, sizeof(ss));
    /* stupid non-const API */
    strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (::WSAStringToAddress(src_copy, af, NULL, (sockaddr*)&ss, &size) == 0) {
        switch (af) {
        case AF_INET:
            *(in_addr*)dst = ((sockaddr_in*)&ss)->sin_addr;
            return 1;
        case AF_INET6:
            *(in6_addr*)dst = ((sockaddr_in6*)&ss)->sin6_addr;
            return 1;
        }
    }

    return 0;
}

const char*
inet_ntop(int af, const void* src, char* dst, socklen_t size)
{
    sockaddr_storage ss;
    unsigned long s = size;

    ::ZeroMemory(&ss, sizeof(ss));
    ss.ss_family = af;

    switch (af) {
    case AF_INET:
        ((sockaddr_in*)&ss)->sin_addr = *(in_addr*)src;
        break;
    case AF_INET6:
        ((sockaddr_in6*)&ss)->sin6_addr = *(in6_addr*)src;
        break;
    default:
        return NULL;
    }

    /* cannot direclty use &size because of strict aliasing rules */
    return
        (::WSAAddressToString((sockaddr*)&ss, sizeof(ss), NULL, dst, &s) == 0) ?
        dst :
        NULL;
}
#endif

namespace nt { namespace http { namespace utility { namespace socket {

void*
get_in_addr(sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in*)sa)->sin_addr);
    }

    return &(((sockaddr_in6*)sa)->sin6_addr);
}

std::string
get_in_ip(sockaddr* sa)
{
    char client_ip[INET6_ADDRSTRLEN];

    inet_ntop(sa->sa_family, get_in_addr(sa), client_ip, sizeof(client_ip));

    return client_ip;
}

unsigned int
get_in_port(sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return ::ntohs(((sockaddr_in*)sa)->sin_port);
    }

    return ::ntohs(((sockaddr_in6*)sa)->sin6_port);
}

int
get_bound_port(SOCKET socket)
{
    sockaddr_in sin;

    socklen_t addrlen = (socklen_t)sizeof(sin);

    if (::getsockname(socket, (sockaddr * ) & sin, &addrlen) != SOCKET_NOERROR) {
        return SOCKET_ERROR;
    }

    return ntohs(sin.sin_port);
}

}}}}
