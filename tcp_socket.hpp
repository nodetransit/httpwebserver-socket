#ifndef HTTPWEBSERVER_TCP_SOCKET_HPP__
#define HTTPWEBSERVER_TCP_SOCKET_HPP__

#include "common.hpp"
#include "interfaces/socket.hpp"

#ifdef LINUX
#    include "linux_tcp_socket.hpp"
#else
#    include "windows_tcp_socket.hpp"
#endif

namespace nt { namespace http {


class __HttpWebServerSocketPort__ TcpSocket :
#ifdef LINUX
      public nt::http::LinuxTcpSocket
#else
      public nt::http:WindowsTcpSocket:
#endif
{
protected:
    int protocol;

public:
    TcpSocket();
};

}}

#endif /* HTTPWEBSERVER_TCP_SOCKET_HPP__ */

