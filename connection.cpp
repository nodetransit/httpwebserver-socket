#include "connection.hpp"

using namespace nt::http;



Connection*
Connection::create_socket()
{
    auto cx = new Connection();

    cx->socket  = std::make_shared<RawSocket>();
    cx->event   = std::make_shared<OverlappedEvent>(cx->socket.get());
    cx->is_read = false;

    return cx;
}

Connection*
Connection::create_socket(std::shared_ptr<RawSocket>& s)
{
    auto cx = new Connection();

    cx->socket  = s;
    cx->event   = std::make_shared<OverlappedEvent>(cx->socket.get());
    cx->is_read = false;

    return cx;
}

Connection*
Connection::create_pipe(const std::string& pipe_name)
{
    auto cx = new Connection();

    cx->pipe    = std::make_shared<Pipe>(pipe_name);
    cx->event   = std::make_shared<OverlappedEvent>();
    cx->is_read = false;

    return cx;
}

namespace nt { namespace http {

std::ostream&
operator<<(std::ostream& out, const Connection& con)
{
    out << "Connection";

    if (!con.name.empty()) {
        out << " \"" << con.name << "\"";

    }

    // out << " (" << static_cast<int>(con.socket) << ")";

    return out;
}

}};
