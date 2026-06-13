#if defined(MOO_LINUX)

#include "mootils/sock/tcp_socket.hpp"
#include <utility>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>   // AF_INET
#include <netinet/tcp.h>  // IPPROTO_TCP
#include <unistd.h>       // close()
#include <errno.h>

namespace sock {
    TCPSocket::TCPSocket() : m_handle(INVALID_SOCKET), m_connected(false) {}

    TCPSocket::~TCPSocket() {
        disconnect();
    }

    TCPSocket::TCPSocket(TCPSocket&& other) noexcept
        : m_handle(std::exchange(other.m_handle, INVALID_SOCKET)),
          m_connected(std::exchange(other.m_connected, false)) {}

    TCPSocket& TCPSocket::operator=(TCPSocket&& other) noexcept {
        if (this != &other) {
            disconnect();
            m_handle = std::exchange(other.m_handle, INVALID_SOCKET);
            m_connected = std::exchange(other.m_connected, false);
        }
        return *this;
    }

    void TCPSocket::disconnect() noexcept {
        shutdown();
        close();
    }

    void TCPSocket::adopt(socket_handle handle, bool connected) noexcept {
        disconnect();
        m_handle = handle;
        m_connected = connected;
    }

    [[nodiscard]] bool TCPSocket::is_connected() const noexcept {
        return m_connected && is_handle_valid();
    }

    SockResult TCPSocket::open() {
        if (is_handle_valid()) {
            return SockResult{ SockErr::DoubleOpen, SockOp::Open, 0, 0 };
        }

        m_handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (is_handle_valid()) {
            return SockResult{ SockErr::None, SockOp::Open, 0, 0 };
        }

        const int err = errno;
        return SockResult{ map_err(err), SockOp::Open, err, 0 };
    }

    bool TCPSocket::is_handle_valid() const noexcept {
        return m_handle != INVALID_SOCKET;
    }

    void TCPSocket::shutdown() noexcept {
        if (is_handle_valid()) {
            ::shutdown(m_handle, SHUT_RDWR);
        }
        m_connected = false;
    }

    void TCPSocket::close() noexcept {
        if (is_handle_valid()) {
            ::close(m_handle);
        }
        m_handle = INVALID_SOCKET;
        m_connected = false;
    }
}
#endif
