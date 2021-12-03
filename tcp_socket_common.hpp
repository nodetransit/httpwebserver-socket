#ifndef HTTPWEBSERVER_TCP_SOCKET_COMMON_HPP__
#define HTTPWEBSERVER_TCP_SOCKET_COMMON_HPP__

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "common.hpp"
#include "interfaces/socket.hpp"
#include "tcp_socket.hpp"
#include "connection.hpp"
#include "timeval.hpp"

namespace nt { namespace http {

typedef void (* event_callback)(void*, void*);

class __HttpWebServerSocketPort__ TcpSocketCommon
{
public:
    // TcpSocketCommon() = default;
    // ~TcpSocketCommon() = default;

    addrinfo* get_addrinfo(TcpSocket*, const char*);
    void bind(TcpSocket*, const char*, const char*);
    void bind(TcpSocket*, const char*, const unsigned short);
    void listen(TcpSocket*, const unsigned int, event_callback);
};

}}

#endif /* HTTPWEBSERVER_TCP_SOCKET_COMMON_HPP__ */
