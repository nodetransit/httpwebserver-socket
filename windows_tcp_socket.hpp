#ifndef HTTPWEBSERVER_WINDOWS_TCP_SOCKET_HPP__
#define HTTPWEBSERVER_WINDOWS_TCP_SOCKET_HPP__

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "common.hpp"
#include "interfaces/socket.hpp"
#include "connection.hpp"
#include "timeval.hpp"

namespace nt { namespace http {

typedef void (* event_callback)(void*, void*);

class __HttpWebServerSocketPort__ WindowsTcpSocket :
      public nt::http::interfaces::Socket
{
private:
    std::string        port;
    unsigned int       queue_count;
    const unsigned int max_connections;

    SOCKET server_pipe;
    SOCKET server_socket;

    bool is_open;

    std::vector<std::shared_ptr<Connection>> connections;

protected:
    int protocol;

public:
    WindowsTcpSocket();
    ~WindowsTcpSocket() noexcept;

    void bind(const char*, const char*);
    void bind(const char*, const unsigned short);
    void listen(const unsigned int, event_callback);
    void open();
    void close();

private:
    addrinfo* get_addrinfo(const char*);
    void create_socket(addrinfo*);
    void close_socket(SOCKET);
    bool select();
    void receive_data(SOCKET);
    void write_data(SOCKET);

};

}}

#endif /* HTTPWEBSERVER_WINDOWS_TCP_SOCKET_HPP__ */

