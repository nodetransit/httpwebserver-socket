#include "socket.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace nt::http;

Socket::Socket() :
    port(0),
    connection_count(0),
#ifdef LOSER
    socket(NULL),
#endif
    is_open(false)
{
#ifdef LOSER
    int     ret;
    WSADATA wsaData;

    if ((ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        std::string error = "WSAStartup failed with error " + ret;

        throw std::runtime_error(error.c_str());
    }

    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket == INVALID_SOCKET) {
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("Invalid socket");
    }

    is_open = true;
#endif

    std::cout << "creating socket" << std::endl;
}

Socket::~Socket() noexcept
{
    if (is_open) {
#ifdef LOSER
        close();
#endif
    }

#ifdef LOSER
    if (::WSACleanup() == SOCKET_ERROR) {
        int         ret   = WSAGetLastError();
        std::string error = "WSACleanup failed with error " + ret;

        std::cerr << error << std::endl;
        // throw std::runtime_error(error.c_str());
    }
#endif
}

void
Socket::bind(const unsigned short p)
{
    port = p;

#ifdef LOSER
    SOCKADDR_IN server_address = {};

    server_address.sin_family      = AF_INET;
    server_address.sin_port        = ::htons(port);
    server_address.sin_addr.s_addr = ::htonl(INADDR_ANY);

    if (::bind(socket, (SOCKADDR*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("Invalid socket");
    }
#endif
}

void
Socket::listen(const unsigned int count, event_callback callback)
{
    connection_count = count;

#ifdef LOSER
    if(::listen(socket, connection_count) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();

        close_socket(socket);

        // todo error handling
        throw std::runtime_error("Invalid socket");
    }
#endif
}

void
Socket::open()
{
#ifdef LOSER
    SOCKET client = ::accept(socket, nullptr, nullptr);

    if(client == INVALID_SOCKET) {
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("unable to create client");
    }

    sockaddr_in client_info     = {};
    int         client_info_len = sizeof(sockaddr_in);

    if (::getpeername(client, (sockaddr*)(&client_info), &client_info_len) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("unable to get name");
    }

    std::string client_ip = ::inet_ntoa(client_info.sin_addr);

    std::string body = "<p>client ip:" + client_ip +
                       "</p>"
                       "<a href=''>refresh</a>"
                       "\r\n";

    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Connection: keep-alive\r\n"
                           "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" +
                           body.c_str();

    ::send(client, response.c_str(), response.size(), 0);
    close_socket(client);
#endif
}

void
Socket::close()
{
    std::cout << "closing socket" << std::endl;
#ifdef LOSER
    close_socket(socket);
#endif

    is_open = false;
}

#ifdef LOSER
void
Socket::close_socket(SOCKET s)
{
    if(::shutdown(s, SD_BOTH) == SOCKET_ERROR) {
        int error = ::WSAGetLastError();
        // todo error handling
    }

    ::closesocket(s);
}
#endif
