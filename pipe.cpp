#include "pipe.hpp"

#include "common.hpp"
#include "timeval.hpp"

using namespace nt::http;

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

    if(::mkfifo(name.c_str(), NUMBER_OF_THE_BEAST) == -1) {
        throw std::runtime_error("Failed to create communication pipe.");
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
    pipe = ::open(name, O_RDWR | O_NONBLOCK);

    if(pipe == -1) {
        throw std::runtime_error("Failed to open communication pipe.");
    }
#endif
}

void
Pipe::close()

{
#ifdef LOSE
    if (_handle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(_handle);
        _handle = INVALID_HANDLE_VALUE;
    }
#else
    if (_handle != -1) {
        ::close(_handle);
        _handle = -1;
    }
#endif
}
