#ifndef HTTPWEBSERVER_SOCKET_HPP__
#define HTTPWEBSERVER_SOCKET_HPP__

#include "interfaces/socket.hpp"

#ifdef LOSER
#   include <winsock2.h>
#   include <ws2tcpip.h>
#endif

namespace nt { namespace http {

typedef void (* event_callback)(void*, void*);

class __HttpWebServerSocketPort__ Socket :
      nt::http::interfaces::Socket
{
private:
    unsigned short port;
    unsigned int connection_count;

#ifdef LOSER
    SOCKET socket;
#endif

    bool is_open;

public:
    Socket();
    virtual ~Socket() noexcept;

    void bind(const unsigned short);
    void listen(const unsigned int, event_callback);
    void open();
    void close();

private:
    void accept();

#ifdef LOSER
    void close_socket(SOCKET);
#endif
};

}}

#endif /* HTTPWEBSERVER_SOCKET_HPP__ */

