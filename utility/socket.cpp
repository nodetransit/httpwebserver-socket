#include "socket.hpp"

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>
#include <macros/repeat_until.hpp>

#include <stdexcept>

#ifdef LOSE
#    include <windows.h>
#else
#    include <errno.h>
#endif


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

static addrinfo*
_get_addrinfo(const char* server_address, const char* port)
{
    addrinfo* server_info;
    addrinfo hints = {0};

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol =  IPPROTO_IP, // ANY TCP
    hints.ai_flags    = AI_PASSIVE;

    int result = 0;

    if ((result = ::getaddrinfo(server_address, port, &hints, &server_info)) != SOCKET_NOERROR) {
        return nullptr;
    }


    return server_info;
}

static inline int
_get_errno()
{
#ifdef lose
    return ::GetLastError();
#else
    return errno;
#endif
}

static inline int
_set_errno(int code)
{
    int prev = _get_errno();

#ifdef lose
    return ::SetLastError(code);
#else
    errno = code;
#endif

    return prev;
}

static inline void
_close_socket(int socket)
{
#ifdef LOSE
    ::shutdown(socket, SD_BOTH);
    ::closesocket(s);
#else
    ::shutdown(socket, SHUT_RDWR);
    ::close(socket);
#endif
}

SOCKET
create_socket(const char* host, const char* service)
{
    addrinfo* server_info = nullptr;

    ______________________________________________________________
              if (server_info != nullptr) {
                  freeaddrinfo(server_info);
              }
    _____________________________________________________________;

    server_info = _get_addrinfo(host, service);

    if (server_info == nullptr) {
        return INVALID_SOCKET;
    }

    int bind_result = SOCKET_NOERROR;
    SOCKET server_socket = INVALID_SOCKET;
    bool is_open = false;
    int last_error = 0;

    for (addrinfo* p = server_info; p != nullptr; p = p->ai_next) {
        last_error = errno;

        if (is_open) {
            _close_socket(server_socket);

            is_open = false;
        }

        continue_if ((server_socket = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET);

#ifdef lose
        static const char ONE             = '1';
        int               REUSE_PORT_ADDR = SO_REUSEADDR;
#else
        static const int  ONE             = 1;
        int               REUSE_PORT_ADDR = SO_REUSEPORT;
#endif

        is_open = true;

        break_if (::setsockopt(server_socket, SOL_SOCKET, REUSE_PORT_ADDR, &ONE, sizeof(ONE)) != SOCKET_ERROR &&
                  (bind_result = ::bind(server_socket, p->ai_addr, p->ai_addrlen)) != SOCKET_ERROR);
    }

    if (server_socket == INVALID_SOCKET) {
        _set_errno(last_error);

        return INVALID_SOCKET;
    }

    if (bind_result == SOCKET_ERROR) {
        last_error = errno;

        _close_socket(server_socket);

        _set_errno(last_error);

        return INVALID_SOCKET;
    }

    return server_socket;
}

SOCKET
create_socket(const char* host, const int port)
{
    return create_socket(host, std::to_string(port).c_str());
}

}}}}
