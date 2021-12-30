#include "pipe.hpp"

#include "common.hpp"
#include "timeval.hpp"

#ifdef LINUX
#    include <sys/stat.h>
#endif


using namespace nt::http;

static inline bool
_file_exists(const char* name)
{
    struct stat buf;

    return ::stat(name, &buf) == 0;
}

Pipe::Pipe(const std::string& pipe_name) :
      name(""),
      handle(_handle),
#ifdef LOSE
      _handle(INVALID_HANDLE_VALUE)
#else
      _handle(-1)
#endif
{
#ifdef LOSE
    name    = "\\\\.\\pipe\\" + pipe_name;
    _handle = ::CreateNamedPipe(name.c_str(),
                                PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                1, // PIPE_UNLIMITED_INSTANCES,
                                0, 0, 1000,
                                nullptr);

    if (_handle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to create communication pipe.");
    }
#else
    static const unsigned int NUMBER_OF_THE_BEAST = 0600;
    name = "/tmp/" + pipe_name;

    if (_file_exists(name.c_str()) && ::unlink(name.c_str()) == -1) {
        throw std::runtime_error("Failed to delete existing communication pipe '" + pipe_name + "'.");
    }

    if (::mkfifo(name.c_str(), NUMBER_OF_THE_BEAST) == -1) {
        throw std::runtime_error("Failed to create communication pipe '" + pipe_name + "'.");
    }

#endif
}

Pipe::~Pipe()
{
    close();
}

void
Pipe::open(OverlappedEvent* event)
{
#ifdef LOSE
    if (!::ConnectNamedPipe(_handle, const_cast<LPOVERLAPPED>(&event->overlapped))) {
        int error = ::GetLastError();

        switch (error) {
        case ERROR_IO_PENDING:
            // std::cout << "The overlapped connection in progress\n";
            break;

        case ERROR_PIPE_CONNECTED:
            // std::cout << "Client is already connected\n";
            ::SetEvent(event->handle);
            break;

        case ERROR_PIPE_LISTENING:
            // std::cout << "Pipe is listening\n";
            break;

        default: {
            throw std::runtime_error("Failed to open communication pipe.");
        }
        }
    }
#else
    _handle = ::open(name.c_str(), O_RDWR | O_NONBLOCK);

    if (_handle == -1) {
        throw std::runtime_error("Failed to open communication pipe.");
    }
#endif
}

void
Pipe::close() noexcept
{
#ifdef LOSE
    if (_handle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(_handle);
        _handle = INVALID_HANDLE_VALUE;
    }

    ::DisconnectNamedPipe(_handle);
#else
    if (_handle != -1) {
        ::close(_handle);
        _handle = -1;
    }

    if (::unlink(name.c_str()) == -1) {
        std::cerr << "Failed to delete existing communication pipe '" << name << "'." << std::endl;
    }
#endif
}
