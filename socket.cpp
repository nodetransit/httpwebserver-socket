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
#elif defined(LINUX)
      socket(0),
#endif
    is_open(false)
{
    create_socket();

    std::cout << "creating socket" << std::endl;
}

Socket::~Socket() noexcept
{
    if (is_open) {
        close();
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
Socket::create_socket()
{
#ifdef LOSER
    int     ret;
    WSADATA wsaData;

    if ((ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        std::string error = "WSAStartup failed with error " + ret;

        throw std::runtime_error(error.c_str());
    }

    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#elif defined(LINUX)
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
#endif

    if (socket == INVALID_SOCKET) {
#ifdef LOSER
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("Invalid socket");
#elif defined(LINUX)
        std::string error = "fail to create socket";

        switch (errno) {
            // todo get error message
        default:
            break;
        }

        throw std::runtime_error(error.c_str());
#endif
    }

    static const int ONE = 1;

#ifdef LOSER
    int setopt_result = ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&ONE, sizeof(ONE));
#elif defined(LINUX)
    int setopt_result = ::setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &ONE, sizeof(ONE));
#endif

    if(setopt_result == SOCKET_ERROR) {
#ifdef LOSER
#elif defined(LINUX)
#endif
        throw std::runtime_error("unable to set socket option");
    }

    is_open = true;
}

void
Socket::bind(const unsigned short p)
{
    port = p;

    sockaddr_in server_address = {};

    server_address.sin_family      = AF_INET;
    server_address.sin_port        = ::htons(port);
    server_address.sin_addr.s_addr = ::htonl(INADDR_ANY);

    if (::bind(socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
#ifdef LOSER
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("Invalid socket");
#elif defined(LINUX)
        std::string error = "fail to bind socket";

        switch (errno) {
            // todo get error message
        default:
            break;
        }

        error += " " + std::to_string(errno);

        throw std::runtime_error(error.c_str());
#endif
    }
}

void
Socket::listen(const unsigned int count, event_callback callback)
{
    connection_count = count;

    int listen_result = ::listen(socket, connection_count);

    if(listen_result == SOCKET_ERROR) {
#ifdef LOSER
        int error = ::WSAGetLastError();

        close_socket(socket);

        // todo error handling
        throw std::runtime_error("Invalid socket");
#elif defined(LINUX)
        std::string error = "fail to create listen";

        switch (errno) {
            // todo get error message
        default:
            break;
        }

        throw std::runtime_error(error.c_str());
#endif
    }
}

void
Socket::open()
{
    SOCKET client = ::accept(socket, nullptr, nullptr);

    if(client == INVALID_SOCKET) {
#ifdef LOSER
        int error = ::WSAGetLastError();
        // todo error handling
        throw std::runtime_error("unable to create client");
#elif defined(LINUX)
        std::string error = "fail to create client";

        switch (errno) {
            // todo get error message
        default:
            break;
        }

        throw std::runtime_error(error.c_str());
#endif
    }

    sockaddr_in client_info     = {};
    int         client_info_len = sizeof(sockaddr_in);

#ifdef LOSER
    int getpeername_result = ::getpeername(client, (sockaddr*)(&client_info), &client_info_len);
#elif defined(LINUX)
    int getpeername_result = ::getpeername(client, (sockaddr*)(&client_info), (socklen_t*)&client_info_len);
#endif

    if (getpeername_result == SOCKET_ERROR) {
#ifdef LOSER
        int error = ::WSAGetLastError();
        // todo error handling
#endif
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

    std::cout << "sending response" << std::endl;

    ::send(client, response.c_str(), response.size(), 0);
    close_socket(client);
}

void
Socket::close()
{
    std::cout << "closing socket" << std::endl;

    close_socket(socket);

    is_open = false;
}

void
Socket::close_socket(SOCKET s)
{
#ifdef LOSER
    int shutdown_result = ::shutdown(s, SD_BOTH);
#elif defined(LINUX)
    int shutdown_result = ::shutdown(s, SHUT_RDWR);
#endif

    if(shutdown_result == SOCKET_ERROR) {
        // todo error handling
#ifdef LOSER
        int error = ::WSAGetLastError();
        throw std::runtime_error("unable to shutdown socket");
#elif defined(LINUX)
        std::string error = "fail to shutdown client";

        switch (errno) {
            // todo get error message
        default:
            break;
        }

        throw std::runtime_error(error.c_str());
#endif
    }

#ifdef LOSER
    ::closesocket(s);
#elif defined(LINUX)
    ::close(s);
#endif
}
