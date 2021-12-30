#include "overlapped_event.hpp"

#include "common.hpp"
#include "interfaces/socket.hpp"
#include "timeval.hpp"

#include <macros/leave_loop_if.hpp>
#include <macros/scope_guard.hpp>
#include <macros/repeat_until.hpp>

using namespace nt::http;


#ifdef LOSE
OverlappedEvent::OverlappedEvent() :
      handle(_handle),
      overlapped(_overlapped),
      _handle(INVALID_HANDLE_VALUE),
      _overlapped({0}),
      socket(nullptr)
{
    _handle = ::CreateEvent(nullptr, true, false, nullptr);

    if (_handle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to create event.");
    }

    _overlapped.hEvent = _handle;
}

OverlappedEvent::OverlappedEvent(RawSocket* socket) :
      handle(_handle),
      overlapped(_overlapped),
      _handle(INVALID_HANDLE_VALUE),
      _overlapped({0}),
      socket(socket)
{
    _handle = ::WSACreateEvent();

    if (_handle == WSA_INVALID_EVENT) {
        throw std::runtime_error("Failed to create event.");
    }
}

OverlappedEvent::~OverlappedEvent() noexcept
{
    if (_handle != INVALID_HANDLE_VALUE) {
        ::CloseHandle(_handle);
    }
}

void
OverlappedEvent::set()
{
    bool is_pipe = socket == nullptr;

    if (is_pipe) {
        ::SetEvent(_overlapped.hEvent);
    } else {
        int select_result = ::WSAEventSelect(socket->socket,
                                             _handle,
                                             FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE
                                            );

        if (select_result == SOCKET_ERROR) {
            throw std::runtime_error("Failed to select WSA event.");
        }

    }
}

void
OverlappedEvent::reset()
{
    bool is_pipe = socket == nullptr;

    if (is_pipe) {
        if (!::ResetEvent(_handle)) {
            throw std::runtime_error("Unable to reset event.");
        }
    } else {
        if (!::WSAResetEvent(_handle)) {
            throw std::runtime_error("Unable to reset wsa event.");
        }
    }
}
#else
OverlappedEvent::OverlappedEvent(RawSocket* socket)
{
}

void
OverlappedEvent::set()
{
}

void
OverlappedEvent::reset()
{
}
#endif
