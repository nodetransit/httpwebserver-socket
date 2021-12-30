#ifndef HTTPWEBSERVER_LINUX_TCP_SOCKET_HPP__
#define HTTPWEBSERVER_LINUX_TCP_SOCKET_HPP__

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "common.hpp"
#include "raw_socket.hpp"
#include "pipe.hpp"
#include "connection.hpp"
#include "interfaces/socket.hpp"
#include "connection.hpp"
#include "timeval.hpp"

namespace nt { namespace http {

typedef void (* event_callback)(void*, void*);

class __HttpWebServerSocketPort__ LinuxTcpSocket :
      public nt::http::interfaces::Socket
{
private:
    std::shared_ptr<Connection> server;
    std::shared_ptr<Connection> pipe;
private:
    unsigned int       queue_count;
    const unsigned int max_connections;

    std::vector<std::shared_ptr<Connection>> connections;

    fd_set read_list;
    fd_set write_list;
    fd_set error_list;

public:
    LinuxTcpSocket();
    ~LinuxTcpSocket() noexcept = default;

    void bind(const char*, const char*);
    void bind(const char*, const unsigned short);
    void listen(const unsigned int, event_callback);
    void open();
    void close();

private:
    void reset_socket_lists();
    unsigned short get_last_socket();
    bool select();
    inline bool is_new_connection(const Connection*);
    void handle_new_connection();
    void receive_data(SOCKET);
    void write_data(SOCKET);
};

}}

#endif /* HTTPWEBSERVER_LINUX_TCP_SOCKET_HPP__ */
