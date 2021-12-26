#ifndef HTTPWEBSERVER_SOCKET_PIPE_HPP__
#define HTTPWEBSERVER_SOCKET_PIPE_HPP__

#include "common.hpp"
#include "overlapped_event.hpp"
#include "interfaces/socket.hpp"

namespace nt { namespace http {


class __HttpWebServerSocketPort__ Pipe
{
private:
    std::string name;

#ifdef LOSE
public:
    const HANDLE& handle;
private:
    HANDLE _handle;
#else
public:
    const int& handle;
private:
    int _handle;
#endif

public:
    Pipe(const std::string&);
    ~Pipe();

    void open(OverlappedEvent*);
    void close();
};

}}

#endif /* HTTPWEBSERVER_SOCKET_PIPE_HPP__ */
