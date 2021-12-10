#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <algorithm>
#include <tinythread.h>

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>
#include <macros/repeat_until.hpp>

#include <utility/socket.hpp>

#include "windows_tcp_socket.hpp"

using namespace nt::http;


#ifdef LINUX
#    define HWS_TCP_SOCKET_COMMON LinuxTcpSocket;
#else
#    define HWS_TCP_SOCKET_COMMON WindowsTcpSocket
#endif

addrinfo*
HWS_TCP_SOCKET_COMMON::get_addrinfo(const char* server_address)
{
    addrinfo* server_info;
    addrinfo hints = {0};

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags    = AI_PASSIVE;

    int result = 0;

    if ((result = ::getaddrinfo(server_address, port.c_str(), &hints, &server_info)) != SOCKET_NOERROR) {
        std::string error = _get_last_error("Failed to get information about the specified network port/service '" + port + "'.");

        throw std::runtime_error(error.c_str());
    }


    return server_info;
}

void
HWS_TCP_SOCKET_COMMON::bind(const char* server_address, const char* service)
{
    addrinfo* server_info = nullptr;

    ______________________________________________________________
              if (server_info != nullptr) {
                  freeaddrinfo(server_info);
              }
    _____________________________________________________________;

    port = service;

    server_info = get_addrinfo(server_address);

    // Connection pipe = create_pipe();

    // connections.push_back(pipe);

    create_socket(server_info);
}

void
HWS_TCP_SOCKET_COMMON::bind(const char* server_address, const unsigned short port_no)
{
    bind(server_address, std::to_string(port_no).c_str());
}

void
HWS_TCP_SOCKET_COMMON::listen(const unsigned int count, event_callback callback)
{
    queue_count = count;

    int listen_result = ::listen(server_socket, queue_count);

    if (listen_result == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to listen to port/service " + port + ".");

        close_socket(server_socket);

        throw std::runtime_error(error.c_str());
    }

    connections.push_back(HWS_TCP_SOCKET_COMMON::_create_connection(server_socket));
}

#undef HWS_TCP_SOCKET_COMMON
