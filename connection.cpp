#include "connection.hpp"

using namespace nt::http;

Connection::Connection(const SOCKET n) :
      close(false),
      socket(n),
#ifdef LOSE
      event(INVALID_HANDLE_VALUE),
      pipe(INVALID_HANDLE_VALUE),
      overlapped(nullptr),
#endif
      is_read(false),
      name("")
{
}

#ifdef LOSE
Connection::Connection(const SOCKET n, const HANDLE h) :
      close(false),
      socket(n),
      event(h),
      pipe(INVALID_HANDLE_VALUE),
      overlapped(nullptr),
      is_read(false),
      name("")
{
}

Connection::Connection(const SOCKET n, const HANDLE h, const HANDLE p, OVERLAPPED* o) :
      close(false),
      socket(n),
      event(h),
      pipe(p),
      overlapped(o),
      is_read(false),
      name("")
{
}
#endif

Connection::~Connection() noexcept
{
    if(socket != INVALID_SOCKET) {
#ifdef LOSE
        ::shutdown(socket, SD_BOTH);
        ::closesocket(socket);
#else
        // todo check if shutdown will cause an error on pipes
        ::shutdown(socket, SHUT_RDWR);
        ::close(socket);
#endif
    }

#ifdef LOSE
    if (event != INVALID_HANDLE_VALUE) {
        ::CloseHandle(event);
    }

    if (overlapped != nullptr) {
        delete overlapped;
    };

    if (pipe != INVALID_HANDLE_VALUE) {
        ::CloseHandle(pipe);
    }
#endif
}

namespace nt { namespace http {

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
