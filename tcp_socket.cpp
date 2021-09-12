#include "tcp_socket.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <tls.h>

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>

using namespace nt::http;

TcpSocket::TcpSocket() : Socket(),
    protocol(IPPROTO_TCP)
{
}

