#include <iostream>
#include <cstdlib>
#include <memory>
#include <exception>
#include <typeinfo>

#include "tcp_socket.hpp"

static void
callback(void*, void*)
{

}

static void
run(nt::http::interfaces::Socket* socket)
{
    // socket->bind("0.0.0.0", "tcp"); // error
    socket->bind("0.0.0.0", "http");
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

int
main(int, char**)
{
    try {
        _main();
        return EXIT_SUCCESS;
    } catch (std::bad_cast& ex) {
        std::cout << "dynamic cast failed" << std::endl;

        return EXIT_FAILURE;
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;

        return EXIT_FAILURE;
    }

}
