#include <iostream>
#include <cstdlib>

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
    socket->close();
}

int
main(int, char**)
{
    nt::http::Socket socket;

    // run(dynamic_cast<nt::http::interfaces::Socket*>(&socket));
    run((nt::http::interfaces::Socket*)&socket);

    return EXIT_SUCCESS;
}
