#include <iostream>
#include <cstdlib>
#include <memory>
#include <exception>
#include <typeinfo>

#include <tls.h>

#include "socket.hpp"

static void
callback(void*, void*)
{

}

static void
_tls()
{
    tls       * tls;
    tls_config* tls_config;

    if (tls_init() != 0) {
        printf("tls_init() failed\n");
        return;
    }

    if ((tls = tls_server()) == NULL) {
        printf("tls_server() failed\n");
        return;
    }

    if ((tls_config = tls_config_new()) == NULL) {
        printf("tls_config_new() failed\n");
        return;
    }

    tls_close(tls);
    tls_free(tls);
    tls_config_free(tls_config);
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
        _tls();
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
