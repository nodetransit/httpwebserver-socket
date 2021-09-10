#ifndef HTTPWEBSERVER_SOCKET_HPP__
#define HTTPWEBSERVER_SOCKET_HPP__

#include "interfaces/socket.hpp"

#ifdef LOSE
#   include <winsock2.h>
#   include <ws2tcpip.h>
#elif defined(LINUX)
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <unistd.h>
#   include <errno.h>
#
#    define SOCKET         int
#    define SOCKET_ERROR   -1
#    define INVALID_SOCKET -1
#endif

namespace nt { namespace http {

typedef void (* event_callback)(void*, void*);

class __HttpWebServerSocketPort__ Socket :
      public nt::http::interfaces::Socket
{
private:
    unsigned short port;
    unsigned int connection_count;

    SOCKET socket;

    bool is_open;

public:
    Socket();
    ~Socket() noexcept;

    void bind(const unsigned short);
    void listen(const unsigned int, event_callback);
    void open();
    void close();

private:
    void create_socket();
    void close_socket(SOCKET);
};

}}

#endif /* HTTPWEBSERVER_SOCKET_HPP__ */

