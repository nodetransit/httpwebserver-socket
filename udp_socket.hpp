#ifndef HTTPWEBSERVER_UDP_SOCKET_HPP__
#define HTTPWEBSERVER_UDP_SOCKET_HPP__

#include "socket.hpp"


namespace nt { namespace http {


class __HttpWebServerSocketPort__ UdpSocket :
      public nt::http::Socket
{
protected:
    int protocol;

public:
    UdpSocket();
};

}}

#endif /* HTTPWEBSERVER_UDP_SOCKET_HPP__ */

