#ifndef HTTPWEBSERVER_OVERLAPPED_EVENT_SOCKET_HPP__
#define HTTPWEBSERVER_OVERLAPPED_EVENT_SOCKET_HPP__

#include "common.hpp"
#include "raw_socket.hpp"
#include "interfaces/socket.hpp"

namespace nt { namespace http {


class __HttpWebServerSocketPort__ OverlappedEvent
{
#ifdef LOSE
public:
    const HANDLE    & handle;
    const OVERLAPPED& overlapped;
private:
    HANDLE _handle;
    OVERLAPPED _overlapped;
    RawSocket * socket;
public:
    OverlappedEvent();
    OverlappedEvent(RawSocket*);
    ~OverlappedEvent();

    void set();
    void reset();
#endif
};

}}

#endif /* HTTPWEBSERVER_OVERLAPPED_EVENT_SOCKET_HPP__ */
