#include <iostream>
#include <cstdlib>
#include <memory>
#include <exception>
#include <typeinfo>

#include "tcp_socket.hpp"
#include "raw_socket.hpp"
#include "pipe.hpp"

static void
callback(void*, void*)
{

}

static void
run(nt::http::interfaces::Socket* socket)
{
    // socket->bind("0.0.0.0", "tcp"); // error
    socket->bind("0.0.0.0", 8888);
    socket->listen(8, callback);
    socket->open();
    // socket->close();
}

static void
_main()
{
    auto socket = std::make_unique<nt::http::TcpSocket>();
    auto s      = dynamic_cast<nt::http::interfaces::Socket*>(socket.get());

    if (s == nullptr) {
        throw std::bad_cast();
    }

    run(s);
}

static inline bool
_is_set(std::vector<int> bits, int set)
{
    bool is_set = false;

    for (auto& bit :bits) {
        is_set |= static_cast<bool>(bit & set);

        if (is_set) break;
    }

    return is_set;
}

static void
_main_raw()
{
    auto socket = std::make_unique<nt::http::RawSocket>();
    auto event  = std::make_unique<nt::http::OverlappedEvent>(socket.get());

    socket->bind("0.0.0.0", 8888);
    socket->listen(8);
    event->set();

#ifdef LOSE
    HANDLE handles[] = {event->handle};

    ::WaitForMultipleObjects(1, handles, false, INFINITE);
    WSANETWORKEVENTS networkEvents;
    if (::WSAEnumNetworkEvents(socket->socket, event->handle, &networkEvents) == SOCKET_ERROR) {
        std::cout << "enumerate socket events error\n";
    }

    if (_is_set({FD_ACCEPT}, networkEvents.lNetworkEvents)) {
        std::cout << "accept\n";

        auto client = socket->accept();

        char buffer[MAX_INPUT] = {0};

        ::recv(client->socket, buffer, sizeof(buffer) - 1, 0);

        std::cout << buffer;
    }
#else
    auto client = socket->accept();

    char buffer[MAX_INPUT] = {0};

    ::recv(client->socket, buffer, sizeof(buffer) - 1, 0);

    std::cout << buffer;
#endif
}

int
main(int, char**)
{
    try {
        _main();
        // _main_raw();
        return EXIT_SUCCESS;
    } catch (std::bad_cast& ex) {
        std::cout << "dynamic cast failed" << std::endl;

        return EXIT_FAILURE;
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;

        return EXIT_FAILURE;
    }

}
