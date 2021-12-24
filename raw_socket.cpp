#include "raw_socket.hpp"

#include "common.hpp"
#include "interfaces/socket.hpp"
#include "timeval.hpp"

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>
#include <macros/repeat_until.hpp>

using namespace nt::http;

#ifdef LOSE
static inline void
_wsa_startup()
{
    WSADATA wsaData = {0};

    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to start up.");
    }
}
#endif

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
    ::closesocket(socket);
#else
    ::shutdown(socket, SHUT_RDWR);
    ::close(socket);
#endif
}

RawSocket::RawSocket() :
      socket(_socket),
      _socket(INVALID_SOCKET),
      port(SOCKET_ERROR)
{
#ifdef LOSE
    _wsa_startup();
#endif
}

RawSocket::RawSocket(int s) :
      socket(_socket),
      _socket(s),
      port(SOCKET_ERROR)
{
#ifdef LOSE
    _wsa_startup();
#endif
}

RawSocket::~RawSocket()
{
    if (_socket != INVALID_SOCKET) {
        _close_socket(_socket);
    }

#ifdef LOSE
    if (::WSACleanup() == SOCKET_ERROR) {
        std::cerr << "Failed to clean up." << std::endl;
        // throw std::runtime_error(error.c_str());
    }
#endif
}

int
RawSocket::get_port()
{
    if (port != SOCKET_ERROR) {
        return port;
    }

    sockaddr_in sin = {0};

    sockaddr* addr = reinterpret_cast<sockaddr*>(&sin);
    socklen_t addrlen = (socklen_t)sizeof(sin);

    if (::getsockname(_socket, addr, &addrlen) != SOCKET_NOERROR) {
        port = SOCKET_ERROR;
    } else {
        port = ntohs(sin.sin_port);
    }

    return port;
}

static addrinfo*
_get_addrinfo(const char* server_address, const char* port)
{
    addrinfo* server_info = nullptr;
    addrinfo hints = {0};

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_IP; // ANY TCP
    hints.ai_flags    = AI_PASSIVE;

    if (::getaddrinfo(server_address, port, &hints, &server_info) != SOCKET_NOERROR) {
        return nullptr;
    } else {
        return server_info;
    }
}

void
RawSocket::bind(const char* host, const char* service)
{
    addrinfo* server_info = nullptr;

    ______________________________________________________________
    if (server_info != nullptr) {
        freeaddrinfo(server_info);
    }
    _____________________________________________________________;

    if(_socket != INVALID_SOCKET) {
        _close_socket(_socket);

        _socket = INVALID_SOCKET;
    }

    server_info = _get_addrinfo(host, service);

    if (server_info == nullptr) {
        throw std::runtime_error("Unable to get host info.");
    }

    int  bind_result = SOCKET_NOERROR;
    int  last_error  = 0;

    for (addrinfo* p = server_info; p != nullptr; p = p->ai_next) {
        last_error = errno;

        if (_socket != INVALID_SOCKET) {
            _close_socket(_socket);

            _set_errno(last_error);
        }

        continue_if ((_socket = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET);

#ifdef LOSE
        static const char ONE             = '1';
        int               REUSE_PORT_ADDR = SO_REUSEADDR;
#else
        static const int  ONE             = 1;
        int               REUSE_PORT_ADDR = SO_REUSEPORT;
#endif

        break_if (::setsockopt(_socket, SOL_SOCKET, REUSE_PORT_ADDR, &ONE, sizeof(ONE)) != SOCKET_ERROR &&
                  (bind_result = ::bind(_socket, p->ai_addr, p->ai_addrlen)) != SOCKET_ERROR);
    }

    if (_socket == INVALID_SOCKET) {
        throw std::runtime_error("Failed to create socket.");
    }

    if (bind_result == SOCKET_ERROR) {
        last_error = errno;

        _close_socket(_socket);

        _socket = INVALID_SOCKET;

        _set_errno(last_error);

        std::string error = std::string("Failed to create bind socket to port/service '") + service + "'.";
        throw std::runtime_error(error.c_str());
    }
}

void
RawSocket::bind(const char* host, const unsigned short port)
{
    bind(host, std::to_string(port).c_str());
}

void
RawSocket::listen(const unsigned int count)
{
    int listen_result = ::listen(_socket, count);

    if (listen_result == SOCKET_ERROR) {
        std::string error = std::string("Failed to listen to port/service '") + std::to_string(port) + "'.";
        throw std::runtime_error(error.c_str());
    }
}

RawSocket*
RawSocket::accept()
{
    sockaddr_storage client_addr = {0};

    sockaddr* addr = reinterpret_cast<sockaddr*>(&client_addr);
    socklen_t storage_size = sizeof(client_addr);

    int client = ::accept(_socket, addr, &storage_size);

    return new RawSocket(client);
}

void
RawSocket::close()
{
    _close_socket(_socket);
}
