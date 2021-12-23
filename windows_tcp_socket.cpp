#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <algorithm>
#include <tinythread.h>
#include <tls.h>

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>
#include <macros/repeat_until.hpp>

#include <utility/socket.hpp>

#include "windows_tcp_socket.hpp"

using namespace nt::http;

namespace {
const unsigned int NUMBER_OF_THE_BEAST = 0600;

const std::string
_get_last_error()
{
    int error = ::WSAGetLastError();

    switch (error) {
    case EACCES: return "Tried to open a read-only file for writing, file's sharing mode does not allow the specified operations, or the given path is a directory.";
    case EEXIST: return "_O_CREAT and _O_EXCL flags specified, but filename already exists.";
    case EINVAL: return "Invalid oflag or pmode argument.";
    case EMFILE: return "No more file descriptors are available (too many files are open).";
    case ENOENT: return "File or path not found.";

    case WSA_INVALID_HANDLE: return "Specified event object handle is invalid.";
    case WSA_NOT_ENOUGH_MEMORY: return "Insufficient memory available.";
    case WSA_INVALID_PARAMETER: return "One or more parameters are invalid.";
    case WSA_OPERATION_ABORTED: return "Overlapped operation aborted.";
    case WSA_IO_INCOMPLETE: return "Overlapped I/O event object not in signaled state.";
    case WSA_IO_PENDING: return "Overlapped operations will complete later.";
    case WSAEINTR: return "Interrupted function call.";
    case WSAEBADF: return "File handle is not valid.";
    case WSAEACCES: return "Permission denied.";
    case WSAEFAULT: return "Bad address.";
    case WSAEINVAL: return "Invalid argument.";
    case WSAEMFILE: return "Too many open files.";
    case WSAEWOULDBLOCK: return "Resource temporarily unavailable.";
    case WSAEINPROGRESS: return "Operation now in progress.";
    case WSAEALREADY: return "Operation already in progress.";
    case WSAENOTSOCK: return "Socket operation on nonsocket.";
    case WSAEDESTADDRREQ: return "Destination address required.";
    case WSAEMSGSIZE: return "Message too long.";
    case WSAEPROTOTYPE: return "Protocol wrong type for socket.";
    case WSAENOPROTOOPT: return "Bad protocol option.";
    case WSAEPROTONOSUPPORT: return "Protocol not supported.";
    case WSAESOCKTNOSUPPORT: return "Socket type not supported.";
    case WSAEOPNOTSUPP: return "Operation not supported.";
    case WSAEPFNOSUPPORT: return "Protocol family not supported.";
    case WSAEAFNOSUPPORT: return "Address family not supported by protocol family.";
    case WSAEADDRINUSE: return "Address already in use.";
    case WSAEADDRNOTAVAIL: return "Cannot assign requested address.";
    case WSAENETDOWN: return "Network is down.";
    case WSAENETUNREACH: return "Network is unreachable.";
    case WSAENETRESET: return "Network dropped connection on reset.";
    case WSAECONNABORTED: return "Software caused connection abort.";
    case WSAECONNRESET: return "Connection reset by peer.";
    case WSAENOBUFS: return "No buffer space available.";
    case WSAEISCONN: return "Socket is already connected.";
    case WSAENOTCONN: return "Socket is not connected.";
    case WSAESHUTDOWN: return "Cannot send after socket shutdown.";
    case WSAETOOMANYREFS: return "Too many references.";
    case WSAETIMEDOUT: return "Connection timed out.";
    case WSAECONNREFUSED: return "Connection refused.";
    case WSAELOOP: return "Cannot translate name.";
    case WSAENAMETOOLONG: return "Name too long.";
    case WSAEHOSTDOWN: return "Host is down.";
    case WSAEHOSTUNREACH: return "No route to host.";
    case WSAENOTEMPTY: return "Directory not empty.";
    case WSAEPROCLIM: return "Too many processes.";
    case WSAEUSERS: return "User quota exceeded.";
    case WSAEDQUOT: return "Disk quota exceeded.";
    case WSAESTALE: return "Stale file handle reference.";
    case WSAEREMOTE: return "Item is remote.";
    case WSASYSNOTREADY: return "Network subsystem is unavailable.";
    case WSAVERNOTSUPPORTED: return "Winsock.dll version out of range.";
    case WSANOTINITIALISED: return "Successful WSAStartup not yet performed.";
    case WSAEDISCON: return "Graceful shutdown in progress.";
    case WSAENOMORE: return "No more results.";
    case WSAECANCELLED: return "Call has been canceled.";
    case WSAEINVALIDPROCTABLE: return "Procedure call table is invalid.";
    case WSAEINVALIDPROVIDER: return "Service provider is invalid.";
    case WSAEPROVIDERFAILEDINIT: return "Service provider failed to initialize.";
    case WSASYSCALLFAILURE: return "System call failure.";
    case WSASERVICE_NOT_FOUND: return "Service not found.";
    case WSATYPE_NOT_FOUND: return "Class type not found.";
    case WSA_E_NO_MORE: return "No more results.";
    case WSA_E_CANCELLED: return "Call was canceled.";
    case WSAEREFUSED: return "Database query was refused.";
    case WSAHOST_NOT_FOUND: return "Host not found.";
    case WSATRY_AGAIN: return "Nonauthoritative host not found.";
    case WSANO_RECOVERY: return "This is a nonrecoverable error.";
    case WSANO_DATA: return "Valid name, no data record of requested type.";
    case WSA_QOS_RECEIVERS: return "QoS receivers.";
    case WSA_QOS_SENDERS: return "QoS senders.";
    case WSA_QOS_NO_SENDERS: return "No QoS senders.";
    case WSA_QOS_NO_RECEIVERS: return "QoS no receivers.";
    case WSA_QOS_REQUEST_CONFIRMED: return "QoS request confirmed.";
    case WSA_QOS_ADMISSION_FAILURE: return "QoS admission error.";
    case WSA_QOS_POLICY_FAILURE: return "QoS policy failure.";
    case WSA_QOS_BAD_STYLE: return "QoS bad style.";
    case WSA_QOS_BAD_OBJECT: return "QoS bad object.";
    case WSA_QOS_TRAFFIC_CTRL_ERROR: return "QoS traffic control error.";
    case WSA_QOS_GENERIC_ERROR: return "QoS generic error.";
    case WSA_QOS_ESERVICETYPE: return "QoS service type error.";
    case WSA_QOS_EFLOWSPEC: return "QoS flowspec error.";
    case WSA_QOS_EPROVSPECBUF: return "Invalid QoS provider buffer.";
    case WSA_QOS_EFILTERSTYLE: return "Invalid QoS filter style.";
    case WSA_QOS_EFILTERTYPE: return "Invalid QoS filter type.";
    case WSA_QOS_EFILTERCOUNT: return "Incorrect QoS filter count.";
    case WSA_QOS_EOBJLENGTH: return "Invalid QoS object length.";
    case WSA_QOS_EFLOWCOUNT: return "Incorrect QoS flow count.";
    case WSA_QOS_EUNKOWNPSOBJ: return "Unrecognized QoS object.";
    case WSA_QOS_EPOLICYOBJ: return "Invalid QoS policy object.";
    case WSA_QOS_EFLOWDESC: return "Invalid QoS flow descriptor.";
    case WSA_QOS_EPSFLOWSPEC: return "Invalid QoS provider-specific flowspec.";
    case WSA_QOS_EPSFILTERSPEC: return "Invalid QoS provider-specific filterspec.";
    case WSA_QOS_ESDMODEOBJ: return "Invalid QoS shape discard mode object.";
    case WSA_QOS_ESHAPERATEOBJ: return "Invalid QoS shaping rate object.";
    case WSA_QOS_RESERVED_PETYPE: return "Reserved policy QoS element type.";

    default: return "Error code: " + std::to_string(error);
    }
}

const std::string
_get_last_error(const std::string& prefix)
{
    return prefix + " " + _get_last_error();
}

const std::string
_get_last_error_message(int error)
{
    LPTSTR buffer;

    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                    NULL,
                    error,
                    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                    (LPTSTR)&buffer,
                    0,
                    NULL);

    std::string message(buffer);
    ::LocalFree(buffer);
    message.erase(message.find_last_not_of(" \n\r\t") + 1);

    return message;
}

const std::string
_get_last_error_message()
{
    int error = ::GetLastError();

    return _get_last_error_message(error);
}

std::unique_ptr<HANDLE[]>
_get_connection_handles(const std::vector<std::shared_ptr<Connection>> &connections)
{
    auto handles = std::make_unique<HANDLE[]>(connections.size());

    int i = 0;
    for (auto &connection : connections) {
        handles[i++] = connection->event;
    }

    return handles;
}

static Connection*
_find_connection_by_event_handle(const std::vector<std::shared_ptr<Connection>> &connections, HANDLE handle)
{
    for (auto &connection : connections) {
        if (connection->event == handle) {
            return connection.get();
        }
    }

    return nullptr;
}

static void
_remove_connection(std::vector<std::shared_ptr<Connection>>& connections, const Connection* con)
{
    auto is_same_pointer = [con](std::shared_ptr<Connection> c) -> bool {
        return &*c == con;
    };

    connections.erase(std::remove_if(connections.begin(),
                                     connections.end(),
                                     is_same_pointer
                                    ));
}

void
_tls()
{
    tls       * tls;
    tls_config* tls_config;

    if (tls_init() != 0) {
        printf("tls_init() failed\n");
        return;
    }

    if ((tls = tls_server()) == nullptr) {
        printf("tls_server() failed\n");
        return;
    }

    if ((tls_config = tls_config_new()) == nullptr) {
        printf("tls_config_new() failed\n");
        return;
    }

    tls_close(tls);
    tls_free(tls);
    tls_config_free(tls_config);
}
}

WindowsTcpSocket::WindowsTcpSocket() :
      port("0"),
      queue_count(0),
      max_connections(FD_SETSIZE - 1),
      server_socket(0),
      is_open(false),
      protocol(0)
{
    _tls();

    connections.reserve(max_connections);

    int     ret;
    WSADATA wsaData;

    if ((ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        std::string error = _get_last_error("Failed to start up.");

        throw std::runtime_error(error.c_str());
    }
}

WindowsTcpSocket::~WindowsTcpSocket() noexcept
{
    if (is_open) {
        close();
    }

#ifdef LOSE
    if (::WSACleanup() == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to clean up.");

        std::cerr << error << std::endl;
        // throw std::runtime_error(error.c_str());
    }
#endif
}

static bool
_connect_to_client(HANDLE pipe, OVERLAPPED* overlapped)
{
    if (!::ConnectNamedPipe(pipe, overlapped)) {
        int error = ::GetLastError();

        switch (error) {
        case ERROR_IO_PENDING:
            // std::cout << "The overlapped connection in progress\n";
            break;

        case ERROR_PIPE_CONNECTED: // Client is already connected, so signal an event.
            if (::SetEvent(overlapped->hEvent)) {
                break;
            }

        case ERROR_PIPE_LISTENING:
            std::cout << "Pipe is listening\n";
            break;

        default: { // If an error occurs during the connect operation...
            std::cout << "Failed to make connection on named pipe. "
                      << _get_last_error_message(error)
                      << std::endl;
            ::CloseHandle(pipe);
            return false;
        }
        }
    }

    return true;
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

    // char tmpname[L_tmpnam] = {0};
    //
    // if(::tmpnam(tmpname) == nullptr) {
    //     std::cout << "unable to create temp name" << std::endl;
    //
    //     return nullptr;
    // }
    //
    // std::cout << "Creating file " << tmpname << std::endl;

    std::string pipe_name  = "server";

    HANDLE pipe = ::CreateNamedPipe(("\\\\.\\pipe\\" + pipe_name).c_str(),
                                    PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                    PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                    1, // PIPE_UNLIMITED_INSTANCES,
                                    0, 0, 1000,
                                    nullptr);

    if (pipe == nullptr || pipe == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create outbound pipe instance. "
                  << _get_last_error_message()
                  << std::endl;
        return nullptr;
    }

    OVERLAPPED* overlapped = new OVERLAPPED();
    overlapped->hEvent = ::CreateEvent(nullptr, true, false, nullptr);
    if (overlapped->hEvent == nullptr) {
        std::cout << "Failed to create event. "
                  << _get_last_error_message()
                  << std::endl;
        return nullptr;
    }

    if (!_connect_to_client(pipe, overlapped)) {
        return nullptr;
    }

    std::cout << "pipe name: " << pipe_name << std::endl;

    return new Connection(INVALID_SOCKET, overlapped->hEvent, pipe, overlapped);
}

void
WindowsTcpSocket::bind(const char* server_address, const char* service)
{
    Connection* pipe = _create_pipe();
    pipe->name = "pipe";

    connections.push_back(std::shared_ptr<Connection>(pipe));

    server_socket = nt::http::utility::socket::create_and_bind_socket(server_address, service);

    if (server_socket == INVALID_SOCKET) {
        std::string error = _get_last_error("Failed to create socket.");

        throw std::runtime_error(error.c_str());
    }

    int bound_port = nt::http::utility::socket::get_bound_port(server_socket);

    if (bound_port == SOCKET_ERROR) {
        std::string error = _get_last_error("Unable to get bound port.");

        throw std::runtime_error(error.c_str());
    }

    port = service;

#ifdef HTTP_WEB_SERVER_SOCKET_DEBUG
    std::cout << "listening to port " << bound_port << std::endl;
#endif

    is_open = true;
}

void
WindowsTcpSocket::bind(const char* server_address, const unsigned short port_no)
{
    bind(server_address, std::to_string(port_no).c_str());
}

static Connection*
_create_connection(SOCKET socket)
{
    //HANDLE ev_handle = ::CreateEvent(nullptr, true, true, nullptr);
    HANDLE ev_handle = ::WSACreateEvent();

    std::cout << "server event handle :" << ev_handle << "\n";

    if(ev_handle == WSA_INVALID_EVENT) {
        std::string error = _get_last_error("Failed to create event.");

        throw std::runtime_error(error.c_str());
    }

    int select_result = ::WSAEventSelect(socket,
                                         ev_handle,
                                         FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE);

    if (select_result == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to select WSA event.");

        throw std::runtime_error(error.c_str());
    }

    tthread::this_thread::sleep_for(tthread::chrono::microseconds(0));

    return new Connection(socket, ev_handle);
}

void
WindowsTcpSocket::listen(const unsigned int count, event_callback callback)
{
    queue_count = count;

    int listen_result = ::listen(server_socket, queue_count);

    if (listen_result == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to listen to port/service " + port + ".");

        close_socket(server_socket);

        throw std::runtime_error(error.c_str());
    }

    auto server_connection = _create_connection(server_socket);
    server_connection->name = "web server";

    connections.push_back(std::shared_ptr<Connection>(server_connection));
}

void
WindowsTcpSocket::receive_data(SOCKET connection)
{
    int         bytes_rx;
    std::string request;

    repeat {
        char             buffer[MAX_INPUT] = {0};
        const static int flags             = 0; //MSG_DONTWAIT;

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
WindowsTcpSocket::write_data(SOCKET connection)
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

    tthread::this_thread::sleep_for(tthread::chrono::microseconds(0));

    std::string body = "<p>client ip: " + std::string(client_ip) + ":" + std::to_string(client_port) +
                       "</p>\n"
                       "<p>host name: " + std::string(hostname) +
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

bool
WindowsTcpSocket::select()
{
    return true;
}

static void
_handle_pipe(Connection* cx)
{
    if (cx->overlapped != nullptr) {
        DWORD bytes_read;
        bool get_result = GetOverlappedResult(cx->event,
                                              cx->overlapped,
                                              &bytes_read,
                                              false);

        bool pipe_closed = false;

        if (get_result) {
            // ::ZeroMemory(cx->overlapped, sizeof(OVERLAPPED));
            // delete cx->overlapped;
            // cx->overlapped = new OVERLAPPED();
            // cx->overlapped->hEvent = cx->event;

            if (!cx->is_read) {
                // will not load buffer if thread does not sleep
                tthread::this_thread::sleep_for(tthread::chrono::microseconds(1));

                char read_buff[100] = {0};

                if (::ReadFile(cx->pipe, read_buff, 99, nullptr, cx->overlapped)) {
                    std::cout << "read " << std::string(read_buff) << std::endl;

                    cx->is_read = true;
                } else {
                    pipe_closed = true;
                    cx->is_read = false;
                }

            } else {
                char read_buff[100] = "reply\n";

                if(!::WriteFile(cx->pipe, read_buff, 100, nullptr, cx->overlapped)) {
                    pipe_closed = true;
                }

                cx->is_read = false;
            }
        } else {
            cx->is_read = false;
        }

        if (!get_result || pipe_closed) {
            // int error = ::GetLastError();
            // std::cout << _get_last_error_message(error) << std::endl;
            std::cout << "reconnecting to pipe\n";
            ::DisconnectNamedPipe(cx->pipe);
            _connect_to_client(cx->pipe, cx->overlapped);
        }
    }
}

inline void
WindowsTcpSocket::reset_wsa_event(HANDLE event)
{
    if (!::WSAResetEvent(event)) {
        std::string error = _get_last_error("Unable to reset event.");

        close_socket(server_socket);
        // ::WSACloseEvent(ev_handle);

        throw std::runtime_error(error.c_str());
    }
}

static inline bool
_is_set(std::vector<int> bits, int set)
{
    bool is_set = false;

    for (auto& bit :bits) {
        is_set |= static_cast<bool>(bit & set);

        break_if(is_set);
    }

    return is_set;
}

void
WindowsTcpSocket::open()
{
    unsigned int ceiling = 0;

    while (true) {
        std::cout << "listening from " << connections.size() << " connections\n";

        auto handles = _get_connection_handles(connections);

        int select_result = ::WaitForMultipleObjects(connections.size(),
                                                     handles.get(),
                                                     false,
                                                     INFINITE);

        if (select_result == WAIT_FAILED) {
            std::string error = _get_last_error("Failed to select.");

            throw std::runtime_error(error.c_str());
        } else if (select_result == WAIT_TIMEOUT) {
            std::string error = _get_last_error("Timeout.");

            close_socket(server_socket);
            // ::WSACloseEvent(ev_handle);

            throw std::runtime_error(error.c_str());
        } else {
            int index = select_result - WSA_WAIT_EVENT_0;
            HANDLE ev_handle = handles.get()[index];

            Connection* cx = _find_connection_by_event_handle(connections, ev_handle);

            continue_if (cx == nullptr);

            std::cout << "event from :" << *cx << std::endl;

            WSANETWORKEVENTS networkEvents;

            if (::WSAEnumNetworkEvents(cx->socket, ev_handle, &networkEvents) == SOCKET_ERROR) {
                ::ResetEvent(cx->pipe);
                _handle_pipe(cx);
                continue;
            }

            reset_wsa_event(cx->event);

            sockaddr_storage client_addr;

            if (_is_set({FD_ACCEPT}, networkEvents.lNetworkEvents)) {
                std::cout << "accept\n";
                socklen_t storage_size = sizeof(client_addr);
                SOCKET    client       = accept(cx->socket, (sockaddr*) & client_addr, &storage_size);

                // receive_data(client);
                // write_data(client);
                // close_socket(client);
                auto client_con = _create_connection(client);
                client_con->name = "client";
                connections.push_back(std::shared_ptr<Connection>(client_con));
            }

            if (_is_set({FD_OOB, FD_READ}, networkEvents.lNetworkEvents)) {
                std::cout << "read\n";

                receive_data(cx->socket);
                cx->is_read = true;
            }

            if (_is_set({FD_WRITE}, networkEvents.lNetworkEvents)) {
                std::cout << "write\n";


                if (cx->is_read) {
                    write_data(cx->socket);
                    close_socket(cx->socket);
                    _remove_connection(connections, cx);
                }
            }

            if (_is_set({FD_CLOSE}, networkEvents.lNetworkEvents)) {
                std::cout << "close\n";
                _remove_connection(connections, cx);
            }

            if (!_is_set({FD_ACCEPT, FD_OOB, FD_READ, FD_WRITE, FD_CLOSE}, networkEvents.lNetworkEvents)) {
                std::cout << "default (" << networkEvents.lNetworkEvents << ")\n";
            }
        }
    }
}

void
WindowsTcpSocket::close()
{
    close_socket(server_socket);

    is_open = false;
}

void
WindowsTcpSocket::close_socket(SOCKET s)
{
    int shutdown_result = ::shutdown(s, SD_BOTH);

    if (shutdown_result == SOCKET_ERROR && ::WSAGetLastError() == WSAENOTCONN) {
        shutdown_result = SOCKET_NOERROR;
    }

    if (shutdown_result == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to shutdown connection.");

        std::cerr << error << std::endl;
        // throw std::runtime_error(error.c_str());
    }

    if (::closesocket(s) == SOCKET_ERROR) {
        std::string error = _get_last_error("Failed to close socket.");

        std::cerr << error << std::endl;
        // throw std::runtime_error(error.c_str());
    }
}
