#include "mootils/sock/utils.hpp"
#include <vector>
#include <cstddef>
#include <thread>
#include "mootils/sock/socket_result.hpp"

namespace sock {
    bool recv_exact(std::shared_ptr<sock::TCPClient> sock, void* buf, std::size_t size) {
        sock::SockResult result = sock->recv_all(buf, size);
        switch (result.code) {
            case sock::SockErr::None: {
                if (result.bytes <= 0) return false; // should never happen
                break;
            }

            case sock::SockErr::WouldBlock: {
                // should never happen with blocking sockets
                std::this_thread::yield();
                break;
            }

            case sock::SockErr::Closed:     // fallthrough
            case sock::SockErr::Unknown:    // fallthrough
            default: return false;
        }
        return true;
    }

    bool drain(std::shared_ptr<sock::TCPClient> sock, std::size_t size) {
        if (size <= 0) return true;
        std::vector<std::byte> buf{size};
        return recv_exact(sock, buf.data(), size);
    }
}