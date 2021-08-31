#include <iostream>
#include <cstdlib>
#include <memory>
#include <exception>
#include <typeinfo>

#include "socket.hpp"

static void
callback(void*, void*)
{

}

static void
run(nt::http::interfaces::Socket* socket)
{
    socket->bind(80);
    socket->listen(8, callback);
    socket->open();
    // socket->close();
}

static void
_main()
{
    auto socket = std::make_unique<nt::http::Socket>();
    auto s      = dynamic_cast<nt::http::interfaces::Socket*>(socket.get());

    if (s == nullptr) {
        throw std::bad_cast();
    }

    run(s);
    // run((nt::http::interfaces::Socket*)socket.get());
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
    }

}
