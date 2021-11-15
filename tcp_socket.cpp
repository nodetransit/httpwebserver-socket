#include "tcp_socket.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>

using namespace nt::http;

TcpSocket::TcpSocket() :
#ifdef LINUX
      LinuxTcpSocket(),
#else
      WindowsTcpSocket(),
#endif
    protocol(IPPROTO_TCP)
{
}

