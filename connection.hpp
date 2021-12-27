#ifndef HTTPWEBSERVER_SOCKET_CONNECTION_HPP__
#define HTTPWEBSERVER_SOCKET_CONNECTION_HPP__

#include <memory>

#include "common.hpp"
#include "raw_socket.hpp"
#include "overlapped_event.hpp"
#include "pipe.hpp"

namespace nt { namespace http {

class Connection
{
public:
    std::shared_ptr<Pipe> pipe;
    std::shared_ptr<OverlappedEvent> event;
    std::shared_ptr<RawSocket> socket;

    /**
     * @brief has date been read from the device
     */
    bool is_read;
    std::string name;

private:
    Connection() = default;

public:
    ~Connection() noexcept = default;

    static Connection* create_socket();
    static Connection* create_socket(std::shared_ptr<RawSocket>&);
    static Connection* create_pipe(const std::string&);


    friend std::ostream& operator<<(std::ostream&, const Connection&);
};

}}

#endif /* HTTPWEBSERVER_SOCKET_CONNECTION_HPP__ */
