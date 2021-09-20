#ifndef HTTPWEBSERVER_SOCKET_HPP_UTILITY__
#define HTTPWEBSERVER_SOCKET_HPP_UTILITY__

#include "../common.hpp"


#ifdef __MINGW32__
int inet_pton(int af, const char* src, void* dst);

const char* inet_ntop(int af, const void* src, char* dst, socklen_t size);
#endif

namespace nt { namespace http { namespace utility { namespace socket {

void* get_in_addr(sockaddr* sa);

inline void*
get_in_addr(sockaddr_storage* sa)
{
    return get_in_addr((sockaddr*)sa);
}

std::string get_in_ip(sockaddr* sa);

inline std::string
get_in_ip(sockaddr_storage* sa)
{
    return get_in_ip((sockaddr*)sa);
}


unsigned int get_in_port(sockaddr* sa);

inline unsigned int
get_in_port(sockaddr_storage* sa)
{
    return get_in_port((sockaddr*)sa);
}

int get_bound_port(SOCKET socket);

}}}}

#endif /* HTTPWEBSERVER_SOCKET_HPP_UTILITY__ */
