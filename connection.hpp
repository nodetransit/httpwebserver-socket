#ifndef HTTPWEBSERVER_SOCKET_CONNECTION_HPP__
#define HTTPWEBSERVER_SOCKET_CONNECTION_HPP__

#include "common.hpp"

namespace nt { namespace http {

class Connection
{
public:
    SOCKET socket;

    Connection() = default;
    Connection(const int);
};

}}

#endif /* HTTPWEBSERVER_SOCKET_CONNECTION_HPP__ */
