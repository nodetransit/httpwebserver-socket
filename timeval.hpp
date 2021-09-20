#ifndef HTTPWEBSERVER_SOCKET_TIMEVAL_HPP__
#define HTTPWEBSERVER_SOCKET_TIMEVAL_HPP__

#include <memory>
#include <ostream>

#include "common.hpp"

namespace nt { namespace http {

class Timeval
{
public:
    const static Timeval INFINITE;
    const static Timeval ZERO;

private:
    std::unique_ptr<timeval> time;

public:
    explicit Timeval(const int);
    explicit Timeval(const double);
    explicit Timeval(const timeval);
    Timeval(const Timeval&);

    operator timeval*() const;

    friend std::ostream& operator<<(std::ostream&, const Timeval&);

    std::string to_string() const;

private:
    Timeval();

};

}}

#endif /* HTTPWEBSERVER_SOCKET_TIMEVAL_HPP__ */
