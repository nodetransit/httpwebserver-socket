#ifndef HTTPWEBSERVER_SOCKET_CONNECTION_HPP__
#define HTTPWEBSERVER_SOCKET_CONNECTION_HPP__

#include "common.hpp"

namespace nt { namespace http {

class Connection
{
public:
    SOCKET socket;
#ifdef LOSE
    HANDLE  event;
#endif
    std::string name;

    Connection() = default;
    Connection(const SOCKET);
#ifdef LOSE
    Connection(const SOCKET, const HANDLE);
#endif

    friend std::ostream& operator<<(std::ostream&, const Connection&);
};

}}

#endif /* HTTPWEBSERVER_SOCKET_CONNECTION_HPP__ */
