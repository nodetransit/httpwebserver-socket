#ifndef HTTPWEBSERVER_TCP_SOCKET_HPP__
#define HTTPWEBSERVER_TCP_SOCKET_HPP__

#include "socket.hpp"


namespace nt { namespace http {


class __HttpWebServerSocketPort__ TcpSocket :
      public nt::http::Socket
{
protected:
    int protocol;

public:
    TcpSocket();
};

}}

#endif /* HTTPWEBSERVER_TCP_SOCKET_HPP__ */

