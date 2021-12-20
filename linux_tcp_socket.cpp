#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <algorithm>
#include <tinythread.h>

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>
#include <macros/repeat_until.hpp>

#include <utility/socket.hpp>

#include "linux_tcp_socket.hpp"

using namespace nt::http;

namespace {
const unsigned int NUMBER_OF_THE_BEAST = 0600;

const std::string
_get_last_error()
{
    int error = errno;

    switch (error) {
    case EPERM: return "Operation not permitted";
    case ENOENT: return "No such file or directory";
    case ESRCH: return "No such process";
    case EINTR: return "Interrupted system call";
    case EIO: return "I/O error";
    case ENXIO: return "No such device or address";
    case E2BIG: return "Argument list too long";
    case ENOEXEC: return "Exec format error";
    case EBADF: return "Bad file number";
    case ECHILD: return "No child processes";
    // case EAGAIN: return "Try again";
    case ENOMEM: return "Out of memory";
    case EACCES: return "Permission denied";
    case EFAULT: return "Bad address";
    case ENOTBLK: return "Block device required";
    case EBUSY: return "Device or resource busy";
    case EEXIST: return "File exists";
    case EXDEV: return "Cross-device link";
    case ENODEV: return "No such device";
    case ENOTDIR: return "Not a directory";
    case EISDIR: return "Is a directory";
    case EINVAL: return "Invalid argument";
    case ENFILE: return "File table overflow";
    case EMFILE: return "Too many open files";
    case ENOTTY: return "Not a typewriter";
    case ETXTBSY: return "Text file busy";
    case EFBIG: return "File too large";
    case ENOSPC: return "No space left on device";
    case ESPIPE: return "Illegal seek";
    case EROFS: return "Read-only file system";
    case EMLINK: return "Too many links";
    case EPIPE: return "Broken pipe";
    case EDOM: return "Math argument out of domain of func";
    case ERANGE: return "Math result not representable";
    case EDEADLK: return "Resource deadlock would occur";
    case ENAMETOOLONG: return "File name too long";
    case ENOLCK: return "No record locks available";
    case ENOSYS: return "Invalid system call number";
    case ENOTEMPTY: return "Directory not empty";
    case ELOOP: return "Too many symbolic links encountered";
    case EWOULDBLOCK: return "Operation would block";
    case ENOMSG: return "No message of desired type";
    case EIDRM: return "Identifier removed";
    case ECHRNG: return "Channel number out of range";
    case EL2NSYNC: return "Level 2 not synchronized";
    case EL3HLT: return "Level 3 halted";
    case EL3RST: return "Level 3 reset";
    case ELNRNG: return "Link number out of range";
    case EUNATCH: return "Protocol driver not attached";
    case ENOCSI: return "No CSI structure available";
    case EL2HLT: return "Level 2 halted";
    case EBADE: return "Invalid exchange";
    case EBADR: return "Invalid request descriptor";
    case EXFULL: return "Exchange full";
    case ENOANO: return "No anode";
    case EBADRQC: return "Invalid request code";
    case EBADSLT: return "Invalid slot";
    case EBFONT: return "Bad font file format";
    case ENOSTR: return "Device not a stream";
    case ENODATA: return "No data available";
    case ETIME: return "Timer expired";
    case ENOSR: return "Out of streams resources";
    case ENONET: return "Machine is not on the network";
    case ENOPKG: return "Package not installed";
    case EREMOTE: return "Object is remote";
    case ENOLINK: return "Link has been severed";
    case EADV: return "Advertise error";
    case ESRMNT: return "Srmount error";
    case ECOMM: return "Communication error on send";
    case EPROTO: return "Protocol error";
    case EMULTIHOP: return "Multihop attempted";
    case EDOTDOT: return "RFS specific error";
    case EBADMSG: return "Not a data message";
    case EOVERFLOW: return "Value too large for defined data type";
    case ENOTUNIQ: return "Name not unique on network";
    case EBADFD: return "File descriptor in bad state";
    case EREMCHG: return "Remote address changed";
    case ELIBACC: return "Can not access a needed shared library";
    case ELIBBAD: return "Accessing a corrupted shared library";
    case ELIBSCN: return ".lib section in a.out corrupted";
    case ELIBMAX: return "Attempting to link in too many shared libraries";
    case ELIBEXEC: return "Cannot exec a shared library directly";
    case EILSEQ: return "Illegal byte sequence";
    case ERESTART: return "Interrupted system call should be restarted";
    case ESTRPIPE: return "Streams pipe error";
    case EUSERS: return "Too many users";
    case ENOTSOCK: return "Socket operation on non-socket";
    case EDESTADDRREQ: return "Destination address required";
    case EMSGSIZE: return "Message too long";
    case EPROTOTYPE: return "Protocol wrong type for socket";
    case ENOPROTOOPT: return "Protocol not available";
    case EPROTONOSUPPORT: return "Protocol not supported";
    case ESOCKTNOSUPPORT: return "Socket type not supported";
    case EOPNOTSUPP: return "Operation not supported on transport endpoint";
    case EPFNOSUPPORT: return "Protocol family not supported";
    case EAFNOSUPPORT: return "Address family not supported by protocol";
    case EADDRINUSE: return "Address already in use";
    case EADDRNOTAVAIL: return "Cannot assign requested address";
    case ENETDOWN: return "Network is down";
    case ENETUNREACH: return "Network is unreachable";
    case ENETRESET: return "Network dropped connection because of reset";
    case ECONNABORTED: return "Software caused connection abort";
    case ECONNRESET: return "Connection reset by peer";
    case ENOBUFS: return "No buffer space available";
    case EISCONN: return "Transport endpoint is already connected";
    case ENOTCONN: return "Transport endpoint is not connected";
    case ESHUTDOWN: return "Cannot send after transport endpoint shutdown";
    case ETOOMANYREFS: return "Too many references: cannot splice";
    case ETIMEDOUT: return "Connection timed out";
    case ECONNREFUSED: return "Connection refused";
    case EHOSTDOWN: return "Host is down";
    case EHOSTUNREACH: return "No route to host";
    case EALREADY: return "Operation already in progress";
    case EINPROGRESS: return "Operation now in progress";
    case ESTALE: return "Stale file handle";
    case EUCLEAN: return "Structure needs cleaning";
    case ENOTNAM: return "Not a XENIX named type file";
    case ENAVAIL: return "No XENIX semaphores available";
    case EISNAM: return "Is a named type file";
    case EREMOTEIO: return "Remote I/O error";
    case EDQUOT: return "Quota exceeded";
    case ENOMEDIUM: return "No medium found";
    case EMEDIUMTYPE: return "Wrong medium type";
    case ECANCELED: return "Operation Canceled";
    case ENOKEY: return "Required key not available";
    case EKEYEXPIRED: return "Key has expired";
    case EKEYREVOKED: return "Key has been revoked";
    case EKEYREJECTED: return "Key was rejected by service";
    case EOWNERDEAD: return "Owner died";
    case ENOTRECOVERABLE: return "State not recoverable";
    case ERFKILL: return "Operation not possible due to RF-kill";
    case EHWPOISON: return "Memory page has hardware error";
    default: return "Error code: " + std::to_string(error);
    }
}

const std::string
_get_last_error(const std::string& prefix)
{
    return prefix + " " + _get_last_error();
}

Connection*
_create_connection(SOCKET socket)
{
    return new Connection(socket);
}
}

LinuxTcpSocket::LinuxTcpSocket() :
      port("0"),
      queue_count(0),
      max_connections(FD_SETSIZE - 1),
      server_socket(0),
      is_open(false)
{
    connections.reserve(max_connections);
}

LinuxTcpSocket::~LinuxTcpSocket() noexcept
{
    if (is_open) {
        close();
    }
}

static Connection*
_create_pipe()
{
    // static const std::vector<std::string> tmpvars = {
    //       "TMPDIR",
    //       "TEMP",
    //       "TMP"
    // };
    //
    // char* tmpdir;
    // for (auto &var : tmpvars) {
    //     tmpdir = std::getenv(var.c_str());
    //
    //     if(tmpdir != nullptr) {
    //         break;
    //     }
    // }
    //
    // if(tmpdir == nullptr) {
    //     std::cerr << "unable to create pipe. cannot find temporary directory." << std::endl;
    //
    //     // return;
    // }

    char tmpname[L_tmpnam] = {0};

    if(::tmpnam(tmpname) == nullptr) {
        std::cout << "unable to create temp name" << std::endl;

        return nullptr;
    }

    std::cout << "Creating file " << tmpname << std::endl;

    int pipe = -1;

    if(::mkfifo(tmpname, NUMBER_OF_THE_BEAST) == -1) {
        std::string error = _get_last_error();

        std::cerr << error << std::endl;

        return nullptr;
    }

    pipe = ::open(tmpname, O_RDONLY | O_NONBLOCK);

    if(pipe == -1) {
        std::string error = _get_last_error();

        std::cerr << error << std::endl;

        return nullptr;
    }

    std::cout << "temp name : " << tmpname << std::endl;

    auto connection = new Connection(pipe);
    connection->name = "pipe";

    return connection;
}

void
LinuxTcpSocket::bind(const char* server_address, const char* service)
{
    auto pipe = _create_pipe();

    if (pipe != nullptr) {
        connections.push_back(std::shared_ptr<Connection>(pipe));
    }

    server_socket = nt::http::utility::socket::create_socket(server_address, service);

    if (server_socket == INVALID_SOCKET) {
        std::string error = _get_last_error("Failed to create socket.");

        throw std::runtime_error(error.c_str());
    }

    int bound_port = nt::http::utility::socket::get_bound_port(server_socket);

    if (bound_port == SOCKET_ERROR) {
        std::string error = _get_last_error("Unable to get bound port.");

        throw std::runtime_error(error.c_str());
    }

#ifdef HTTP_WEB_SERVER_SOCKET_DEBUG
    std::cout << "listening to port " << bound_port << std::endl;
#endif

    is_open = true;
}

void
LinuxTcpSocket::bind(const char* server_address, const unsigned short port_no)
{
    bind(server_address, std::to_string(port_no).c_str());
}

void
LinuxTcpSocket::listen(const unsigned int count, event_callback callback)
{
    queue_count = count;

    int listen_result = ::listen(server_socket, queue_count);

    if (listen_result == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to listen to port/service " + port + ".");

        close_socket(server_socket);

        throw std::runtime_error(error.c_str());
    }

    auto con = _create_connection(server_socket);
    con->name = "web server";

    connections.push_back(std::shared_ptr<Connection>(con));
}

void
LinuxTcpSocket::handle_connection()
{
    sockaddr_storage client_addr;

    socklen_t storage_size = sizeof(client_addr);
    SOCKET    client       = ::accept(server_socket, (sockaddr*)&client_addr, &storage_size);

    if (client == INVALID_SOCKET) {
        std::string error = _get_last_error("Failed to accept from client.");

        // todo: do not throw, just close the socket or what not
        throw std::runtime_error(error.c_str());
    }

    FD_SET(client, &read_list);

    auto con = _create_connection(client);
    con->name = "client";

    connections.push_back(std::shared_ptr<Connection>(con));

    {
        std::string  client_ip   = nt::http::utility::socket::get_in_ip(&client_addr);
        unsigned int client_port = nt::http::utility::socket::get_in_port(&client_addr);

#ifdef HTTP_WEB_SERVER_SOCKET_DEBUG
        std::cout << "------------------------------\n"
                  << "server [" << server_socket << "] has "
                  << "new connection from " << client_ip << ":" << client_port
                  << " [" << client << "]"
                  << std::endl;
#endif
    }
}


void
LinuxTcpSocket::receive_data(SOCKET connection)
{
    int         bytes_rx;
    std::string request;

    repeat {
        char             buffer[MAX_INPUT] = {0};
        const static int flags             = MSG_DONTWAIT;

        tthread::this_thread::sleep_for(tthread::chrono::microseconds(1));

        if ((bytes_rx = ::recv(connection, buffer, sizeof(buffer) - 1, flags)) == SOCKET_ERROR) {
            std::string error = _get_last_error("Failed to receive data.");

            // todo: do not throw, just close the socket or what not
            // throw std::runtime_error(error.c_str());
            break;
        }

        buffer[bytes_rx] = '\0';

        request += std::string(buffer);

        if (request.size() >= 4) {
            // signifies the end of the headers
            // if there is post data, one needs to check the
            // Content-Length
            std::string end    = request.substr(request.size() - 4, 4);
            bool        is_end = end == "\r\n\r\n";

            if (is_end) {
                // FD_CLR(connection, &read_list);
                break;
            }
        }
    } until(bytes_rx == 0);

#ifdef HTTP_WEB_SERVER_SOCKET_DEBUG
    std::cout << "received request from [" << connection << "]"
              << std::endl
              << request
              << std::endl;
#endif
}

void
LinuxTcpSocket::write_data(SOCKET connection)
{
    sockaddr_storage client_addr;

    socklen_t storage_size = sizeof(client_addr);

    ::getpeername(connection, (sockaddr*)&client_addr, &storage_size);

    std::string  client_ip   = nt::http::utility::socket::get_in_ip(&client_addr);
    unsigned int client_port = nt::http::utility::socket::get_in_port(&client_addr);

    auto hostname = (char*)calloc(HOST_NAME_MAX + 1, sizeof(char));
    if (::gethostname(hostname, HOST_NAME_MAX) == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to get host name.");

        // todo: do not throw, just close the socket or what not
        throw std::runtime_error(error.c_str());
    }

    tthread::this_thread::sleep_for(tthread::chrono::microseconds(1));

    std::string body = "<p>client ip: " + std::string(client_ip) + ":" + std::to_string(client_port) +
                       "</p>\n"
                       "<p>host name: " + std::string(hostname) +
                       "</p>\n"
                       "<p>"
                       "request " + std::to_string(rand() % 100) +
                       "</p>\n"
                       "\r\n";

    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=UTF-8\r\n"
                           "Connection: keep-alive\r\n"
                           "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" +
                           body.c_str();

    ::free(hostname);
    ::send(connection, response.c_str(), response.size(), 0);

#ifdef HTTP_WEB_SERVER_SOCKET_DEBUG
    std::cout << "writing response"
              << "to [" << connection << "]"
              << std::endl;
#endif
}

void
LinuxTcpSocket::reset_socket_lists()
{
    FD_ZERO(&read_list);
    FD_ZERO(&write_list);
    FD_ZERO(&error_list);

    auto closed = [](const std::shared_ptr<Connection>& c) -> bool {
        return c->socket == INVALID_SOCKET;
    };

    connections.erase(std::remove_if(connections.begin(),
                                     connections.end(),
                                     closed),
                      connections.end());

    for (auto& connection: connections) {
        // continue_if(connection.socket == INVALID_SOCKET);

        FD_SET(connection->socket, &read_list);
        FD_SET(connection->socket, &write_list);
        FD_SET(connection->socket, &error_list);
    }
}

unsigned short
LinuxTcpSocket::get_last_socket()
{
    unsigned int last_socket = server_socket;

    for (auto& connection: connections) {
        if (connection->socket > last_socket) {
            last_socket = connection->socket;
        }
    }

    return last_socket;
}

bool
LinuxTcpSocket::select()
{
    reset_socket_lists();

    unsigned short last_socket = get_last_socket();
    Timeval timeout = Timeval::Infinite;

    int select_result;

#ifdef HTTP_WEB_SERVER_SOCKET_DEBUG
    std::cout << "selecting from " << connections.size() << " connection(s)\n";
#endif

    if ((select_result = ::select(last_socket + 1, &read_list, &write_list, &error_list, timeout)) == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to poll connections.");

        close_socket(server_socket);

        throw std::runtime_error(error.c_str());
    }

    return select_result == 0;
}

inline bool
LinuxTcpSocket::is_new_connection(const Connection* connection) {
    return connection->socket == server_socket;
}

void
LinuxTcpSocket::open()
{
    unsigned int ceiling = 0;

    while (true) {
        bool leave = false;

        continue_if (select());

        for (auto& connection : connections) {
            continue_if (connection->socket == INVALID_SOCKET);

            if (FD_ISSET(connection->socket, &read_list)) {
                if (is_new_connection(connection.get())) {
                    if (ceiling < max_connections) {
                        handle_connection();
                        ceiling++;
                    }

                    break;
                } else {
                    receive_data(connection->socket);

                    FD_CLR(connection->socket, &read_list);
                }
            }

            if (FD_ISSET(connection->socket, &write_list)) {
                write_data(connection->socket);
                close_socket(connection->socket);
                connection->socket = INVALID_SOCKET;
                ceiling--;

                FD_CLR(connection->socket, &write_list);
            }

            if (FD_ISSET(connection->socket, &error_list)) {
                std::cout << _get_last_error("Socket exception.") << std::endl;

                close_socket(connection->socket);
                connection->socket = INVALID_SOCKET;
                ceiling--;

                FD_CLR(connection->socket, &error_list);
            }
        }

        break_if(leave);
    }
}

void
LinuxTcpSocket::close()
{
    close_socket(server_socket);

    is_open = false;
}

void
LinuxTcpSocket::close_socket(SOCKET s)
{
    int shutdown_result = ::shutdown(s, SHUT_RDWR);

    if(shutdown_result == SOCKET_ERROR && errno == ENOTCONN) {
        shutdown_result = SOCKET_NOERROR;
    }

    if (shutdown_result == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to shutdown connection.");

        std::cerr << error << std::endl;
        // throw std::runtime_error(error.c_str());
    }

    ::close(s);
}
