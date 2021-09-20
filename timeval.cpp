#include <cmath>

#include <iostream>
#include "timeval.hpp"

namespace nt { namespace http {

Timeval::Timeval(const int seconds) :
      time(std::make_unique<timeval>())
{
    time->tv_sec  = seconds;
    time->tv_usec = 0;
}

Timeval::Timeval(const double seconds) :
      time(std::make_unique<timeval>())
{
    double integral;

    time->tv_sec    = static_cast<int>(seconds);
    double fraction = std::modf(seconds, &integral);
    time->tv_usec   = static_cast<int>(fraction * 1e6);
}

Timeval::Timeval(const timeval t) :
      time(std::make_unique<timeval>())
{
    time->tv_sec  = t.tv_sec;
    time->tv_usec = t.tv_usec;
}

Timeval::Timeval(const Timeval& t) :
    time(nullptr)
{
    if (t.time != nullptr) {
        time = std::make_unique<timeval>();

        time->tv_sec  = t.time->tv_sec;
        time->tv_usec = t.time->tv_usec;
    }
}

Timeval::Timeval() :
    time(nullptr)
{
}

Timeval::operator timeval*() const
{
    return time.get();
}

std::string
Timeval::to_string() const
{
    if (time == nullptr) {
        return "NULL";
    } else {
        //int decimal = time->tv_usec / 1e6;
        int decimal = time->tv_usec;

        return std::to_string(time->tv_sec) +
              "." +
              std::to_string(decimal);
    }
}

const Timeval Timeval::INFINITE;
const Timeval Timeval::ZERO  = Timeval((int)0);

std::ostream&
operator<<(std::ostream& stream, const nt::http::Timeval& t)
{
    stream << t.to_string();

    return stream;
}

}}

