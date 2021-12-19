#ifndef HTTPWEBSERVER_SOCKET_CONNECTION_HPP__
#define HTTPWEBSERVER_SOCKET_CONNECTION_HPP__

#include "common.hpp"

namespace nt { namespace http {

class Connection
{
public:
    bool close;
    SOCKET socket;
#ifdef LOSE
    HANDLE  event;
    HANDLE  pipe;
    OVERLAPPED* overlapped;
    bool is_reading;
#endif
    std::string name;

    Connection() = default;
    Connection(const SOCKET);
#ifdef LOSE
    Connection(const SOCKET, const HANDLE);
    Connection(const SOCKET, const HANDLE, const HANDLE, OVERLAPPED*);
#endif
    ~Connection() noexcept;

    friend std::ostream& operator<<(std::ostream&, const Connection&);
};

}}

#endif /* HTTPWEBSERVER_SOCKET_CONNECTION_HPP__ */
