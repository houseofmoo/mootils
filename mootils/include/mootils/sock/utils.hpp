#pragma once

#include <memory>
#include <cstdint>
#include "mootils/sock/tcp_socket.hpp"

namespace sock {
    // recv exactly size bytes, return false if failure (likely disconnected), true on success
    bool recv_exact(std::shared_ptr<sock::TCPClient> sock, void* buf, std::size_t size);

    // drain size bytes from tcp client recv buffer
    bool drain(std::shared_ptr<sock::TCPClient> sock, std::size_t size);
}
