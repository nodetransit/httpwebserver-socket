#include "udp_socket.hpp"

using namespace nt::http;

UdpSocket::UdpSocket() : Socket(),
    protocol(IPPROTO_UDP)
{
}

