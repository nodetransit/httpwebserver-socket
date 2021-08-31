#ifndef HTTPWEBSERVER_SOCKET_HPP__
#define HTTPWEBSERVER_SOCKET_HPP__

#include "interfaces/socket.hpp"

namespace nt { namespace http {

typedef void (* event_callback)(void*, void*);

class __HttpWebServerSocketPort__ Socket :
      nt::http::interfaces::Socket
{
private:
    unsigned short port;
    unsigned int connection_count;

public:
    Socket();
    virtual ~Socket() noexcept;

    void bind(const unsigned short);
    void listen(const unsigned int, event_callback);
    void open();
    void close();

private:
    void accept();
};

}}

#endif /* HTTPWEBSERVER_SOCKET_HPP__ */

