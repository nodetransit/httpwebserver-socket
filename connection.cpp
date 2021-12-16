#include "connection.hpp"

using namespace nt::http;

Connection::Connection(const SOCKET n) :
      socket(n),
      name("")
{
}

#ifdef LOSE
Connection::Connection(const SOCKET n, const HANDLE h) :
      socket(n),
      event(h)
{
}
#endif

namespace nt { namespace  http {

std::ostream&
operator<<(std::ostream& out, const Connection& con)
{
    out << "Connection";

    if (!con.name.empty()) {
        out << " \"" << con.name << "\"";

    }

    out << " (" << static_cast<int>(con.socket) << ")";

    return out;
}

}};
