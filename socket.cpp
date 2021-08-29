#include "socket.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace nt::http;

Socket::Socket()
{
#ifdef LOSER
    int     ret;
    WSADATA wsaData;

    if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        std::string error = "WSAStartup failed with error " + ret;

        throw std::runtime_error(error.c_str());
    }
#endif

    std::cout << "socket" << std::endl;
}

Socket::~Socket()
{
#ifdef LOSER
    if (WSACleanup() == SOCKET_ERROR) {
        int         ret   = WSAGetLastError();
        std::string error = "WSACleanup failed with error " + ret;

        std::cerr << error << std::endl;
        // throw std::runtime_error(error.c_str());
    }
#endif
}
