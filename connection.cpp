#include "connection.hpp"

using namespace nt::http;

Connection::Connection(const int n) :
      socket(n)
{
}
