#include "socket.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace nt::http;

Socket::Socket() :
    port(80),
    connection_count(8)
{
#ifdef LOSER
    int     ret;
    WSADATA wsaData;

    if ((ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        std::string error = "WSAStartup failed with error " + ret;

        throw std::runtime_error(error.c_str());
    }

    SOCKET server = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server == INVALID_SOCKET) {
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("Invalid socket");
    }

    SOCKADDR_IN server_address = {};

    server_address.sin_family      = AF_INET;
    server_address.sin_port        = ::htons(port);
    server_address.sin_addr.s_addr = ::htonl(INADDR_ANY);

    if (::bind(server, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("Invalid socket");
    }

    if(::listen(server, connection_count) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();
        // todo error handling, close server socket
        throw std::runtime_error("Invalid socket");
    }

    SOCKET client = ::accept(server, nullptr, nullptr);

    if(client == INVALID_SOCKET) {
        int error = ::WSAGetLastError();
        // todo error handling
    }

    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Connection: keep-alive\r\n"
                           "Content-Length: 0\r\n\r\n";

    ::send(client, response.c_str(), response.size(), 0);

    if(::shutdown(client, SD_BOTH) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();
        // todo error handling
    }

    ::closesocket(client);

    if(::shutdown(server, SD_BOTH) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();
        // todo error handling
    }

    ::closesocket(server);
#endif

    std::cout << "socket" << std::endl;
}

Socket::~Socket() noexcept
{
#ifdef LOSER
    if (::WSACleanup() == SOCKET_ERROR) {
        int         ret   = WSAGetLastError();
        std::string error = "WSACleanup failed with error " + ret;

        std::cerr << error << std::endl;
        // throw std::runtime_error(error.c_str());
    }
#endif
}
