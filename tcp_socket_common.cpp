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

#include "tcp_socket_common.hpp"

using namespace nt::http;

addrinfo*
TcpSocketCommon::get_addrinfo(TcpSocket* tcp, const char* server_address)
{
    addrinfo* server_info;
    addrinfo hints = {0};

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags    = AI_PASSIVE;

    int result = 0;

    if ((result = ::getaddrinfo(server_address, tcp->port.c_str(), &hints, &server_info)) != SOCKET_NOERROR) {
        // std::string error = _get_last_error("Failed to get information about the specified network port/service '" + tcp->port + "'.");
        std::string error = "Failed to get information about the specified network port/service '" + tcp->port + "'.";

        throw std::runtime_error(error.c_str());
    }


    return server_info;
}

void
TcpSocketCommon::bind(TcpSocket* tcp, const char* server_address, const char* service)
{
    addrinfo* server_info = nullptr;

    ______________________________________________________________
              if (server_info != nullptr) {
                  freeaddrinfo(server_info);
              }
    _____________________________________________________________;

    tcp->port = service;

    server_info = get_addrinfo(tcp, server_address);

    // Connection pipe = create_pipe();
    //
    // connections.push_back(pipe);
    //
    // create_socket(server_info);
}

void
TcpSocketCommon::bind(TcpSocket* tcp, const char* server_address, const unsigned short port_no)
{
    bind(tcp, server_address, std::to_string(port_no).c_str());
}

void
TcpSocketCommon::listen(TcpSocket* tcp, const unsigned int count, event_callback callback)
{
    // queue_count = count;

    int listen_result = ::listen(tcp->server_socket, tcp->queue_count);

    if (listen_result == SOCKET_ERROR) {
        // std::string error = _get_last_error("Failed to listen to port/service " + t.port + ".");
        std::string error = "Failed to listen to port/service " + tcp->port + ".";

        // close_socket(t.server_socket);

        throw std::runtime_error(error.c_str());
    }

    // connections.push_back(_create_connection(server_socket));
}
