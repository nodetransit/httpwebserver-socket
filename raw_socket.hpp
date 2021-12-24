#ifndef HTTPWEBSERVER_RAW_SOCKET_HPP__
#define HTTPWEBSERVER_RAW_SOCKET_HPP__

#include "common.hpp"
#include "interfaces/socket.hpp"

namespace nt { namespace http {


class __HttpWebServerSocketPort__ RawSocket
{
public:
    const int& socket;
private:
    int _socket;
    int port;
public:
    RawSocket();
    RawSocket(int);
    ~RawSocket();

    int get_port();
    void bind(const char*, const char*);
    void bind(const char*, const unsigned short);
    void listen(const unsigned int);
    RawSocket* accept();
    void close();
};

}}

#endif /* HTTPWEBSERVER_RAW_SOCKET_HPP__ */
