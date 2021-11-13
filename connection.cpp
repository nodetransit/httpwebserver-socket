#include "connection.hpp"

using namespace nt::http;

Connection::Connection(const SOCKET n) :
      socket(n)
{
}

#ifdef LOSE
Connection::Connection(const SOCKET n, const HANDLE h) :
      socket(n),
      event(h)
{
}
#endif
